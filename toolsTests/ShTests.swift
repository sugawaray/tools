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
    }
    
    override func tearDown() {
        super.tearDown()
    }

    func testProcDummyCmd() {
        let r = o.proc(Sh.dummycmd)
        let s = o.cmd()
        XCTAssertEqual(0, r)
        XCTAssertEqual(Sh.dummycmd, s)
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
