
import * as vscode from 'vscode';
import * as os from 'os';
import * as fs from 'fs';
import { integer } from 'vscode-languageclient';
import * as extension from './extension';

export class DebugAdapterExecutableFactory implements vscode.DebugAdapterDescriptorFactory 
{
	createDebugAdapterDescriptor(_session: vscode.DebugSession, executable: vscode.DebugAdapterExecutable | undefined): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
		// param "executable" contains the executable optionally specified in the package.json (if any)

        if (os.platform() == 'win32') 
        {
            let agkPath : string = extension.getAGKPath();
            if ( agkPath == "" )
            {
                vscode.window.showErrorMessage( "Could not find AppGameKit Studio install folder, please set the path in the extension settings" );
                return;
            }

            let adapterPath : string = agkPath + extension.DebugAdapterPath;
            if ( !fs.existsSync( adapterPath ) )
            {
                vscode.window.showErrorMessage( "Could not find AppGameKit Debug Adapter, please check that you are using the latest version of AppGameKit Studio" );
                return;
            }

            return new vscode.DebugAdapterExecutable( adapterPath );
        }
        else if (os.platform() == 'darwin') 
        {
            vscode.window.showErrorMessage("Debugger is not yet avaliable for Mac");
        }
        else if (os.platform() == 'linux') 
        {
            vscode.window.showErrorMessage("Debugger is not yet avaliable for Linux");
        }
        else
        {
            vscode.window.showErrorMessage("Debugger is not supported on this platform");
        }

		return null;
	}
}


export function getBroadcastDebugConfig( ip : string ) : vscode.DebugConfiguration | undefined
{
    let debugConfiguration: vscode.DebugConfiguration = 
    {
        type: 'agkdebug',
        name: 'AGK Broadcast',
        request: 'launch',
        mode: 'broadcast'
    };

    const project : vscode.WorkspaceFolder | undefined = extension.getCurrentFolder();
    if ( !project ) return undefined;

    debugConfiguration.project = project.uri.fsPath;
    debugConfiguration.targetIP = ip;

    return debugConfiguration;
}

export class AGKDebugProvider implements vscode.DebugConfigurationProvider {
	provideDebugConfigurations(folder: vscode.WorkspaceFolder | undefined, token?: vscode.CancellationToken | undefined): vscode.ProviderResult<vscode.DebugConfiguration[]> 
    {
        //vscode.window.showInformationMessage( "Providing Debug Configurations" );

        //const debugConfiguration = getDebugConfig( 0, "" );        

        //return [ debugConfiguration ];
        return [];
	}

	resolveDebugConfiguration(folder: vscode.WorkspaceFolder | undefined, debugConfiguration: vscode.DebugConfiguration, token?: vscode.CancellationToken | undefined): vscode.ProviderResult<vscode.DebugConfiguration> 
    {
        //vscode.window.showInformationMessage( "Resolving Debug Configuration" );

        if ( debugConfiguration && debugConfiguration.type && debugConfiguration.type != "" ) return debugConfiguration;
        else
        {
            vscode.window.showErrorMessage( "Debug configuration not found, please create a launch.json file with an 'agkdebug' configuration" );
            return undefined;
        }
    }

    resolveDebugConfigurationWithSubstitutedVariables(folder: vscode.WorkspaceFolder | undefined, debugConfiguration: vscode.DebugConfiguration, token?: vscode.CancellationToken | undefined): vscode.ProviderResult<vscode.DebugConfiguration> 
    {
        //vscode.window.showInformationMessage( "Resolving Substitute Debug Configuration" );

        //return getDebugConfig( 0, "" );

        return debugConfiguration;
    }
}