//
//  Term.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/11.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import Foundation

func getlastline(_ txt:String) -> String? {
    if txt.hasSuffix("\n") {
        let rs = txt.characters.reversed();
        var s = ""
        var fnd = false
        var i = rs.startIndex
        while true {
            let bi = i.base
            let be = txt.characters.index(bi, offsetBy: 2, limitedBy: txt.characters.endIndex)
            if be != nil {
                s = txt.substring(with: bi..<be!)
                if s == "$ " {
                    fnd = true
                    s = txt.substring(
                        with: txt.characters.index(bi, offsetBy: 2)..<txt.characters.index(txt.characters.endIndex, offsetBy: -1))
                    break;
                }
            }
            if i == rs.endIndex {
                break;
            }
            i = rs.index(i, offsetBy: 1)
        }
        if fnd {
            return s
        } else {
            return String(txt.characters.dropLast())
        }
    } else {
        return nil
    }
}

class Termtxt {

    func append(_ v:String) -> Void {
        s += v
    }

    func get() -> String {
        return s + input
    }
    
    func getinput() -> String {
        return input
    }
    
    func replace(_ rng:NSRange, _ v:String) -> Bool {
        let l = s.characters.count
        let atend = rng.location >= l
        if (atend) {
            let be = rng.location - l
            var en = be + rng.length
            if (en > input.characters.count) {
                en = input.characters.count
            }
            let beit = input.index(input.startIndex, offsetBy: be)
            let enit = input.index(input.startIndex, offsetBy: en)
            let r: Range<String.Index> = Range<String.Index>(uncheckedBounds: (lower: beit, upper: enit))
            input.replaceSubrange(r, with: v)
        } else {
            let be = s.index(s.startIndex, offsetBy: rng.location)
            let en = s.index(s.startIndex, offsetBy: rng.location + rng.length)
            let r: Range<String.Index> = Range<String.Index>(uncheckedBounds: (lower: be, upper: en))
            s.replaceSubrange(r, with: v)
        }
        return true
    }
    
    func fix() {
        s += input
        input = ""
    }
    
    var s = ""
    var input = ""
}


