//
//  ShDummyCmdTests.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/15.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import XCTest

class ShDummyCmdTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
    }
    
    override func tearDown() {
        o.cleanup()
        super.tearDown()
    }
    
    func testReadStdin() {
        XCTAssertFalse(o.fail)
        o.proc(Sh.dummycmd)
        putsonin("input\n")
        var s: String? = nil
        var ts = timespec()
        ts.tv_sec = 0
        ts.tv_nsec = 10000000
        while (true) {
            nanosleep(&ts, nil)
            s = o.getdummycmdinput()
            if (s != nil && s!.characters.count > 0) {
                break
            }
        }
        XCTAssertEqual("input", s)
    }
    
    var o = Sh()
}
