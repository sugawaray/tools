//
//  CtlTest.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/08/31.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import XCTest
import tools

class CtlTest: XCTestCase {
    
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
        inst = Ctl()
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    func testProcc() {
        var a = Cmd()
        a.id = Cmdid.Nop.rawValue
        if let o = inst {
            o.procc(a)
            o.proc()
            let s = o.status()
            XCTAssertEqual(0, s.0)
            XCTAssertEqual(0, s.1)
        }
    }
    
    func testStatusWithoutCmd() {
        if let o = inst {
            let s = o.status()
            XCTAssertEqual(-1, s.0)
        }
    }
    
    func testStatusBeforeProc() {
        var a = Cmd()
        a.id = Cmdid.Nop.rawValue
        if let o = inst {
            o.procc(a)
            let s = o.status()
            XCTAssertEqual(-1, s.0)
        }
    }
    
    func testStatusAftNextProcc() {
        var a = Cmd()
        a.id = Cmdid.Nop.rawValue
        if let o = inst {
            o.procc(a)
            o.proc()
            o.procc(a)
            let s = o.status()
            XCTAssertEqual(-1, s.0)
        }
    }
    
    func testProccHasAlreadyHad() {
        var a = Cmd()
        a.id = Cmdid.Nop.rawValue
        if let o = inst {
            o.procc(a)
            let r = o.procc(a)
            XCTAssertEqual(-1, r)
        }
    }
    
    func testProcAnotherCmd() {
        var a = Cmd()
        a.id = Cmdid.NopFail.rawValue
        if let o = inst {
            o.procc(a)
            o.proc()
            let s = o.status()
            XCTAssertEqual(0, s.0)
            XCTAssertEqual(-1, s.1)
        }
    }
    
    func testPerformanceExample() {
        // This is an example of a performance test case.
        self.measureBlock {
            // Put the code you want to measure the time of here.
        }
    }
    
    var inst: Ctl? = nil
}
