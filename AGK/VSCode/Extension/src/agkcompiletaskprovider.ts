
import * as vscode from 'vscode';
import { integer } from 'vscode-languageclient';
import * as extension from './extension';
import * as fs from 'fs';

export class AGKCompileTaskProvider implements vscode.TaskProvider {
	provideTasks() 
    {
        let task : vscode.Task | undefined = getCompileTask( 0 );
        if ( !task ) return [];
        return [ task ];
	}

	resolveTask(task: vscode.Task): vscode.Task | undefined 
    {
        return getCompileTask( 0 );
	}
}

export function getCompileTask( run : integer ) : vscode.Task | undefined
{
    let agkPath : string = extension.getAGKPath();
    if ( agkPath == "" )
    {
        vscode.window.showErrorMessage( "Could not find AppGameKit Studio install folder, please set the path in the extension settings" );
        return undefined;
    }

    let compilerPath : string = extension.getAGKPath() + extension.CompilerPath;
    if ( !fs.existsSync( compilerPath ) )
    {
        compilerPath = extension.getAGKPath() + extension.CompilerPathOld;
        if ( !fs.existsSync( compilerPath ) )
        {
            vscode.window.showErrorMessage( "Could not find AppGameKit Compiler, please check that you are using the latest version of AppGameKit Studio, and that your install folder path is correct in the extension settings" );
            return;
        }
    }

    let folder : vscode.WorkspaceFolder | undefined = extension.getCurrentFolder();
    if ( !folder )
    {
        vscode.window.showErrorMessage( "Failed to get workspace folder for current file" );
        return undefined;
    }

    let parameters : string[] = [];
    if ( run == 1 ) parameters.push( "-run" );
    parameters.push( "-64" );
    parameters.push( folder.uri.fsPath );

    let process : vscode.ProcessExecution = new vscode.ProcessExecution( compilerPath, parameters );

    let task : vscode.Task = new vscode.Task(
        { 
            type: "agkcompile"
        },
        vscode.TaskScope.Workspace,
        'Compile Project',
        'AppGameKit',
        process,
        ["$agkcompilematcher", "$agkcompilematcher2"]
    );

    task.presentationOptions.clear = true;
    task.presentationOptions.echo = false;
    task.presentationOptions.reveal = vscode.TaskRevealKind.Never;

    return task;
}