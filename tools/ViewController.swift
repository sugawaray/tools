//
//  ViewController.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/07/04.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UITextViewDelegate {
   
    // MARK: properties
    @IBOutlet weak var txtv: UITextView!

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        regnoti()
        txtv.delegate = self
        txtv.text.append("$ ")
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func prout() {
        var b: Array<CChar> = Array<CChar>(repeating: 0, count: 512);
        while getsfromout(&b, 512) == 0 {
            var s = String(cString: b);
            s = s.trimmingCharacters(
                in: CharacterSet.newlines);
            s = String(format: "%@\n", s)
            txtv.text.append(s)
        }
        txtv.text.append("$ ")
    }
    
    func onKbShow(_ a:NSNotification) {
        let f = ((a as NSNotification).userInfo![UIKeyboardFrameEndUserInfoKey] as? NSValue)?.cgRectValue
        if f != nil {
            let v = UIEdgeInsetsMake(0, 0, f!.height, 0)
            txtv.contentInset = v
            txtv.scrollIndicatorInsets = v
            txtv.scrollRangeToVisible(txtv.selectedRange)
        }
    }
    
    func regnoti() {
        let n = NotificationCenter.default
        n.addObserver(self, selector: #selector(ViewController.onKbShow(_:)),
                      name: NSNotification.Name.UIKeyboardDidShow, object: nil)
    }

    // MARK: UITextViewDelegate
    func textViewDidChange(_ textView: UITextView) {
        let r = getlastline(txtv.text)
        if (r != nil) {
            var cmd = Cmd()
            cmd.id = Cmdid.shcmd.rawValue
            cmd.astr = r
            getInst().ctl.procc(cmd)
            
            DispatchQueue.global(qos: DispatchQoS.QoSClass.utility).async {
                getInst().ctl.proc()
                DispatchQueue.main.async {
                    let s = getInst().ctl.status()
                    if (s.0 == 0) {
                        //print("command done")
                        self.prout()
                    } else {
                        //print("command failed")
                    }
                }
            }
        }
    }
}

