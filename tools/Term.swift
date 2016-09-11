//
//  Term.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/11.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import Foundation

func getlastline(txt:String) -> String? {
    if txt.hasSuffix("\n") {
        let rs = txt.characters.reverse()
        var s = ""
        var fnd = false
        var i = rs.startIndex
        while true {
            let bi = i.base
            let be = bi.advancedBy(2, limit: txt.characters.endIndex)
            s = txt.substringWithRange(bi..<be)
            if s == "$ " {
                fnd = true
                s = txt.substringWithRange(
                    bi.advancedBy(2)..<txt.characters.endIndex.advancedBy(-1))
                break;
            }
            if i == rs.endIndex {
                break;
            }
            i = i.advancedBy(1)
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

