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
        txtv.text.appendContentsOf("$ ")
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func prout() {
        var b: Array<CChar> = Array<CChar>(count: 512, repeatedValue: 0);
        while mgets(&b, 512) != nil {
            var s = String.fromCString(b);
            s = s?.stringByTrimmingCharactersInSet(
                NSCharacterSet.newlineCharacterSet());
            s = String(format: "%@\n", s!)
            txtv.text.appendContentsOf(s!)
        }
        txtv.text.appendContentsOf("$ ")
    }
    
    func onKbShow(a:NSNotification) {
        if let f = a.userInfo![UIKeyboardFrameEndUserInfoKey]?.CGRectValue() {
            let v = UIEdgeInsetsMake(0, 0, f.height, 0)
            txtv.contentInset = v
            txtv.scrollIndicatorInsets = v
            txtv.scrollRangeToVisible(txtv.selectedRange)
        }
    }
    
    func regnoti() {
        let n = NSNotificationCenter.defaultCenter()
        n.addObserver(self, selector: #selector(ViewController.onKbShow(_:)),
                      name: UIKeyboardDidShowNotification, object: nil)
    }

    // MARK: UITextViewDelegate
    func textViewDidChange(textView: UITextView) {
        let t = txtv.text
        let vl = t.unicodeScalars
        if (vl.last == nil) {
            return;
        }
        let nl = "\n".unicodeScalars.last!
        if vl.last == nl {
            let seq = vl.reverse().enumerate()
            var t = -1
            for (i, v) in seq {
                if i == 0 {
                    continue
                } else if v == nl {
                    t = i
                    break;
                }
            }
            var subs = ""
            if t >= 0 {
                subs = String(vl.suffix(t).dropLast())
                print(subs)
            } else {
                subs = String(vl.dropLast())
            }
            var cmd = Cmd()
            cmd.id = Cmdid.Shcmd.rawValue
            cmd.astr = subs
            getInst().ctl.procc(cmd)
            
            dispatch_async(dispatch_get_global_queue(QOS_CLASS_UTILITY, 0)) {
                getInst().ctl.proc()
                dispatch_async(dispatch_get_main_queue()) {
                    let s = getInst().ctl.status()
                    if (s.0 == 0) {
                        print("command done")
                        self.prout()
                    } else {
                        print("command failed")
                    }
                }
            }
        }
    }
}

