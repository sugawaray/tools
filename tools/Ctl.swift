//
//  Ctl.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/08/31.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import Foundation

enum Cmdid : Int {
    case nop    = 1,
    nopFail,
    shcmd,
    savefile,
    invalid
}

enum Ctlv : Int {
    case stInvalid  = -0x10000
}

struct Cmd {
    var id: Int = Cmdid.invalid.rawValue
    var astr: String?
}

class Ctl {
    
    init() {
        cmd = nil
        cst = Ctlv.stInvalid.rawValue
    }
    
    func cleanup() {
        if (sh.running()) {
            sh.kill()
        }
        sh.cleanup()
    }

    func procc(_ carg: Cmd) -> Int {
        if (cmd != nil && cmd!.id == Cmdid.shcmd.rawValue && carg.id == Cmdid.shcmd.rawValue) {
            let s = carg.astr! + "\n"
            putsonin(s)
            return 0
        } else if (cmd != nil) {
            return -1
        } else {
            cmd = carg
            cst = Ctlv.stInvalid.rawValue
            return 0
        }
    }
    
    func proc() {
        if cmd == nil {
            return;
        } else if (cmd!.id == Cmdid.shcmd.rawValue && sh.running()) {
            return;
        } else if cmd!.id != Cmdid.invalid.rawValue {
            switch cmd!.id {
            case Cmdid.nop.rawValue:
                cst = 0
                break
            case Cmdid.nopFail.rawValue:
                cst = -1
                break
            case Cmdid.savefile.rawValue:
                cst = savefile(cmd!.astr!)
                break
            case Cmdid.shcmd.rawValue:
                cst = sh.proc(cmd!.astr!)
                break;
            default:
                cst = Ctlv.stInvalid.rawValue
            }
            if (cmd!.id != Cmdid.shcmd.rawValue) {
                cmd = nil
            } else {
                if (!sh.running()) {
                    cmd = nil
                }
            }
        }
    }
    
    func status() -> (Int, Int) {
        if cst != Ctlv.stInvalid.rawValue {
            return (0, cst)
        } else {
            return (-1, 0)
        }
    }
    
    func savefile(_ path: String) -> Int {
        print("savefile called ", path)
        let m = FileManager.default
        var dst = m.urls(for: FileManager.SearchPathDirectory.documentDirectory,
                                     in:FileManager.SearchPathDomainMask.userDomainMask).first
        
        dst = dst!.appendingPathComponent("tmp.bin")
        print("dst ", dst)
        let src = URL(string: path)!
        do {
            try m.copyItem(at: src, to: dst!)
        } catch let e as NSError {
            print(e.description)
            return -1
        }
        return 0
    }
    
    var cmd: Cmd?
    var cst: Int
    var sh = Sh()
}
