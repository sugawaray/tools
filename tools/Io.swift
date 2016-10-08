//
//  Io.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/08.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import Foundation

func Dbgout1(_ fmt: String, _ args: [CVarArg]) -> Void {
    withVaList(args) { (ap) -> Void in
        dbgput(fmt, ap)
    }
}
