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
        let r = o.proc("ShDummyCommand")
        let s = o.cmd()
        XCTAssertEqual(0, r)
        XCTAssertEqual("ShDummyCommand", s)
    }
    
    func testDoNotProcDummyCmdSimultaneously() {
        o.proc("ShDummyCommand")
        let r = o.proc("ShDummyCommand")
        XCTAssertEqual(-1, r)
    }
    
    func testKillProc() {
        o.proc("ShDummyCommand")
        o.kill()
        let s = o.cmd()
        let r = o.proc("ShDummyCommand")
        XCTAssertEqual(0, r)
        XCTAssertNil(s)
    }
    
    var o = Sh()
}
