//
//  NativeViewController.swift
//  agk_interpreter
//
//  Created by Paul on 24/10/2022.
//

import UIKit

class NativeViewController: UIViewController {
    
    static var sharedViewController : NativeViewController? = nil;
    
    @objc static func isOpen() -> Bool
    {
        return sharedViewController != nil;
    }
    
    @objc static func getSharedViewController() -> NativeViewController
    {
        if ( sharedViewController == nil )
        {
            let storyboard = UIStoryboard(name:"MainStoryboard", bundle:nil);
            sharedViewController = storyboard.instantiateViewController(withIdentifier: "NativeViewController1") as? NativeViewController;
        }
        
        return sharedViewController!;
    }

    override func viewDidLoad() {
        NSLog( "Native view did load" );
        NativeViewController.sharedViewController = self;
        super.viewDidLoad()
    }
    
    @IBAction func didButtonTap()
    {
        guard let agkViewController = AGKViewController.getShared() else { return };
        present( agkViewController, animated:true );
    }

}
