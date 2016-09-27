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

