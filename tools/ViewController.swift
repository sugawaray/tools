//
//  ViewController.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/07/04.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import UIKit
import Darwin;

class ViewController: UIViewController, UITextViewDelegate {
   
    // MARK: properties
    @IBOutlet weak var txtv: UITextView!
    
    func apptxt(_ v:String) -> Void {
        termtxt.append(v);
        txtv.text = termtxt.get()
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        regnoti()
        txtv.delegate = self
        DispatchQueue.global(qos: DispatchQoS.QoSClass.utility).async {
            self.waitoutput()
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func waitoutput() {
        var b: Array<CChar> = Array<CChar>(repeating: 0, count: 512);
        var ts = timespec()
        ts.tv_sec = 0
        ts.tv_nsec = 50000000
        while (true) {
            nanosleep(&ts, nil)
            if ((getsfromout(&b, 512) != 0) && (getsfromerr(&b, 512) != 0)) {
                continue
            }
            let s = String(cString: b);
            DispatchQueue.main.async {
                self.apptxt(s)
            }
        }
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
    func textView(_ textView: UITextView, shouldChangeTextIn range: NSRange, replacementText text: String) -> Bool {
        return termtxt.replace(range, text)
    }
    
    func textViewDidChange(_ textView: UITextView) {
#if true
        var r = termtxt.getinput()
#else
        let r = getlastline(txtv.text)
#endif
        if (r == "") {
            return;
        }
        if (r.characters.last! != "\n") {
            return;
        }
        r = r.substring(to: r.index(before: r.endIndex))
        termtxt.fix()
        var cmd = Cmd()
        cmd.id = Cmdid.shcmd.rawValue
        cmd.astr = r
        getInst().ctl.procc(cmd)
        
        DispatchQueue.global(qos: DispatchQoS.QoSClass.utility).async {
            Dbgout1("DBG:proc pre\n", [])
            getInst().ctl.proc()
            Dbgout1("DBG:proc post\n", [])
        }
    }
    
    var termtxt = Termtxt()
}

