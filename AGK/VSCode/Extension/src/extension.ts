
import * as vscode from 'vscode';
import { integer, LanguageClient, LanguageClientOptions, ServerOptions, VersionedTextDocumentIdentifier, WorkspaceFolder } from 'vscode-languageclient/node';
import { ChildProcess, spawn } from "child_process";
import { AGKCompileTaskProvider, getCompileTask } from './agkcompiletaskprovider';
import { AGKDebugProvider, getBroadcastDebugConfig, DebugAdapterExecutableFactory } from './agkdebugprovider';
import * as fs from 'fs';

let client: LanguageClient;
let server: ChildProcess;

let disposableAgkCompileTaskProvider: vscode.Disposable | undefined;
let disposableAgkDebugProvider : vscode.Disposable | undefined;
let disposableAgkDebugFactory: vscode.Disposable | undefined;
let agkDebugSession : vscode.DebugSession | undefined = undefined;
let agkDebugSessionType : integer;

export const CompilerPath : string = "/media/compiler/AGKCompiler64.exe";
export const CompilerPathOld : string = "/media/compiler/AGKCompiler.exe";
export const DebugAdapterPath : string = "/VSCode/DebugAdapter64.exe";
export const LSPPath : string = "/VSCode/LanguageServer64.exe";

export function activate(context: vscode.ExtensionContext) 
{
    startServer();

    context.subscriptions.push( vscode.commands.registerCommand('agklang.restartServer', commandRestartServer) );
    context.subscriptions.push( vscode.commands.registerCommand('agklang.compile', commandCompile) );
    context.subscriptions.push( vscode.commands.registerCommand('agklang.run', commandRun) );
    context.subscriptions.push( vscode.commands.registerCommand('agklang.broadcast', commandBroadcast) );
    context.subscriptions.push( vscode.commands.registerCommand('agklang.debug', commandDebug) );

    disposableAgkCompileTaskProvider = vscode.tasks.registerTaskProvider( "agkcompile", new AGKCompileTaskProvider() );
    context.subscriptions.push( disposableAgkCompileTaskProvider );

    disposableAgkDebugProvider = vscode.debug.registerDebugConfigurationProvider( "agkdebug", new AGKDebugProvider() );
    context.subscriptions.push( disposableAgkDebugProvider );

    disposableAgkDebugFactory = vscode.debug.registerDebugAdapterDescriptorFactory('agkdebug', new DebugAdapterExecutableFactory() );
	context.subscriptions.push( disposableAgkDebugFactory );

    vscode.debug.onDidStartDebugSession( (debugSession) => { agkDebugSession = debugSession; } );
    vscode.debug.onDidTerminateDebugSession( (debugSession) => 
    { 
        if ( debugSession == agkDebugSession ) 
        { 
            agkDebugSession = undefined; 
            if ( agkDebugSessionType == 1 ) vscode.window.setStatusBarMessage( "Broadcasting Finished" ); 
            else if ( agkDebugSessionType == 2 ) vscode.window.setStatusBarMessage( "Debugging Finished" ); 
            else vscode.window.setStatusBarMessage( "" ); 
        }
    });
}

export function deactivate()
{
    if ( disposableAgkCompileTaskProvider ) disposableAgkCompileTaskProvider.dispose();
    if ( disposableAgkDebugProvider ) disposableAgkDebugProvider.dispose();
    if ( disposableAgkDebugFactory ) disposableAgkDebugFactory.dispose();
    killServer();
}

export function getCurrentFolder() : vscode.WorkspaceFolder | undefined
{
    if ( vscode.workspace.workspaceFolders?.length == 1 )
    {
        return vscode.workspace.workspaceFolders[0];
    }

    let fileURI : vscode.Uri | undefined = vscode.window.activeTextEditor?.document.uri;
    if ( !fileURI ) 
    {
        vscode.window.showErrorMessage( "Failed to get current file path" );
        return undefined;
    }
    let folder : vscode.WorkspaceFolder | undefined = vscode.workspace.getWorkspaceFolder( fileURI );
    if ( !folder )
    {
        return undefined;
    }

    return folder;
}

function commandCompile()
{
    const task : vscode.Task | undefined = getCompileTask( 0 );
    if ( !task ) return;
    
    vscode.window.setStatusBarMessage( "Compiling..." );
    vscode.tasks.executeTask( task );
    vscode.tasks.onDidEndTask( e => {
        if ( e.execution.task == task )
        {
            vscode.window.setStatusBarMessage( "Compile Finished" );
        }
    });
}

function commandRun()
{
    let task : vscode.Task | undefined = getCompileTask( 1 );
    if ( !task ) return;
    
    vscode.window.setStatusBarMessage( "Compiling and Running..." );
    vscode.tasks.executeTask( task );
    vscode.tasks.onDidEndTask( e => {
        if ( e.execution.task == task )
        {
            vscode.window.setStatusBarMessage( "Compile and Run Finished" );
        }
    });
}

function commandBroadcast()
{
    if ( agkDebugSession )
    {
        if ( agkDebugSessionType == 1 ) vscode.debug.stopDebugging( agkDebugSession );
        else vscode.window.showErrorMessage( "Cannot broadcast whilst a debug session is running" );
        return;
    }

    let folder : vscode.WorkspaceFolder | undefined = getCurrentFolder();
    if ( !folder )
    {
        vscode.window.showErrorMessage( "Failed to get workspace folder for current file" );
        return;
    }

    const config = vscode.workspace.getConfiguration("agklang");
    var ipConfig : string | undefined = config.get("broadcastIP");
    if ( !ipConfig ) ipConfig = "";

    const debugConfig : vscode.DebugConfiguration | undefined = getBroadcastDebugConfig( ipConfig );
    if ( !debugConfig )
    {
        vscode.window.showErrorMessage( "Failed to get broadcast configuration" );
        return;
    }

    const task : vscode.Task | undefined = getCompileTask( 0 );
    if ( !task ) return;

    agkDebugSessionType = 1;

    vscode.window.setStatusBarMessage( "Compiling..." );
    vscode.tasks.executeTask( task );
    vscode.tasks.onDidEndTask( e => {
        if ( e.execution.task == task )
        {
            vscode.window.setStatusBarMessage( "Broadcasting..." );
            vscode.debug.startDebugging( folder, debugConfig );
        }
    });
}

function commandDebug()
{
    if ( agkDebugSession )
    {
        if ( agkDebugSessionType == 2 ) vscode.debug.stopDebugging( agkDebugSession );
        else vscode.window.showErrorMessage( "Cannot debug whilst a broadcast session is running" );
        return;
    }

    let folder : vscode.WorkspaceFolder | undefined = getCurrentFolder();
    if ( !folder )
    {
        vscode.window.showErrorMessage( "Failed to get workspace folder for current file" );
        return;
    }

    const task : vscode.Task | undefined = getCompileTask( 0 );
    if ( !task ) return;

    agkDebugSessionType = 2;

    vscode.window.setStatusBarMessage( "Compiling..." );
    vscode.tasks.executeTask( task );
    vscode.tasks.onDidEndTask( e => {
        if ( e.execution.task == task )
        {
            vscode.window.setStatusBarMessage( "Debugging..." );
            vscode.debug.startDebugging( folder, "AGK Debug" );
        }
    });
}

function commandRestartServer()
{
    killServer();
    startServer();
}

function startServer() 
{
    let agkPath : string = getAGKPath();
    if ( agkPath == "" )
    {
        vscode.window.showErrorMessage( "Could not find AppGameKit Studio install folder, please set the path in the extension settings" );
        return;
    }

    let serverPath : string = agkPath + LSPPath;
    if ( !fs.existsSync( serverPath ) )
    {
        vscode.window.showErrorMessage( "Could not find AppGameKit Language Server, please check that you are using the latest version of AppGameKit Studio" );
        return;
    }

    const serverOptions: ServerOptions = async (): Promise<ChildProcess> => {
        server = spawn(serverPath);
        //window.showInformationMessage("Started AppGameKit language server");
        return server;
    };

    const clientOptions: LanguageClientOptions = {
        documentSelector: [ 'agksrc' ],
        diagnosticCollectionName: 'agklang',
    };

    client = new LanguageClient('agklang', 'AppGameKit Language Client', serverOptions, clientOptions);

    client.start();
}

function killServer() {
    if ( client ) 
    { 
        client.stop(); 
    }
    if ( server )
    {
        server.kill();
    }
}

export function getAGKPath()
{
    const config = vscode.workspace.getConfiguration("agklang");
    const pathConfig : string | undefined = config.get("agkPath");
    const pathConfig2 : string | undefined = process.env.AGK_STUDIO_INSTALL_PATH;

    let agkPath : string = "";
    if ( pathConfig ) agkPath = pathConfig;
    else if ( pathConfig2 ) agkPath = pathConfig2;
    
    return agkPath;
}
