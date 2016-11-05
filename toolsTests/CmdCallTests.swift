//
//  CmdCallTests.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/11/05.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import XCTest

class CmdCallTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
        o = Sh()
    }
    
    override func tearDown() {
        o!.cleanup()
        super.tearDown()
    }
    
    func testJustCall() {
        let copyres = copyToDocDir(name: "abc", ofType: "txt", inDirectory: "CmdCallTestsTestd")
        XCTAssertTrue(copyres)
        o!.proc("grep abc abc.txt")
        var b: Array<CChar> = Array<CChar>(repeating: 0, count: 512);
        var ts = timespec()
        ts.tv_sec = 0
        ts.tv_nsec = 50000000
        var i = 0
        var s = ""
        while (i < 30) {
            nanosleep(&ts, nil)
            if ((getsfromout(&b, 512) != 0) && (getsfromerr(&b, 512) != 0)) {
                i += 1
                continue
            }
            s += String(cString: b);
            i += 1
        }
        XCTAssertTrue(s.contains("abc"))
    }

    var o: Sh?
}
