//
//  ShTests.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/15.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import XCTest

class ShTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
        o = Sh()
    }
    
    override func tearDown() {
        o.cleanup()
        super.tearDown()
    }

    func testProcDummyCmd() {
        let r = o.proc(Sh.dummycmd)
        let f = o.running()
        let s = o.cmd()
        XCTAssertEqual(0, r)
        XCTAssertEqual(Sh.dummycmd, s)
        XCTAssertEqual(true, f)
    }
    
    func testDoNotProcDummyCmdSimultaneously() {
        o.proc(Sh.dummycmd)
        let r = o.proc(Sh.dummycmd)
        XCTAssertEqual(-1, r)
    }
    
    func testKillProc() {
        o.proc(Sh.dummycmd)
        o.kill()
        let s = o.cmd()
        let r = o.proc(Sh.dummycmd)
        XCTAssertEqual(0, r)
        XCTAssertNil(s)
    }
    
    var o = Sh()
}
