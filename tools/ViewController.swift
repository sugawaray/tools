//
//  ViewController.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/07/04.
//  Copyright © 2016年 Yutaka Sugawara. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UITextViewDelegate {
   
    // MARK: properties
    @IBOutlet weak var txtv: UITextView!

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        txtv.delegate = self
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
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
            if t >= 0 {
                let subs = String(vl.suffix(t).dropLast())
                print(subs)
            } else {
                print(String(vl.dropLast()))
            }
        }
    }
}

