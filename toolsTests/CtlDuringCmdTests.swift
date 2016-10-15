//
//  CtlDuringCmdTests.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/15.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import XCTest

class CtlDuringCmdTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
        o = Ctl()
    }
    
    override func tearDown() {
        o!.cleanup()
        super.tearDown()
    }
    
    func testPassInputToRunningCmd() {
        var c = Cmd(id: Cmdid.shcmd.rawValue, astr: Sh.dummycmd)
        o!.procc(c)
        o!.proc()
        c = Cmd(id: Cmdid.shcmd.rawValue, astr: "input 1:\n")
        o!.procc(c)
        
        var ts = timespec()
        ts.tv_sec = 0
        ts.tv_nsec = 10000000
        while (o!.sh.getdummycmdinput().compare("") == ComparisonResult.orderedSame) {
            nanosleep(&ts, nil)
        }
        XCTAssertEqual("input 1:", o!.sh.getdummycmdinput())
    }

    var o: Ctl?
}
