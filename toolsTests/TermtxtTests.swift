//
//  TermtxtTests.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/12.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import XCTest

class TermtxtTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
        o = Termtxt()
    }
    
    override func tearDown() {
        super.tearDown()
    }
    
    func testInitialValue() {
        XCTAssertEqual("", o.get())
        XCTAssertEqual("", o.getinput())
    }
    
    func testApp() {
        o.append("append value")
        XCTAssertEqual("append value", o.get())
        XCTAssertEqual("", o.getinput())
    }
    
    func testAppWhenHasVal() {
        o.append("append value 1:")
        o.append("append value 2:")
        XCTAssertEqual("append value 1:append value 2:", o.get())
        XCTAssertEqual("", o.getinput())
    }
    
    func testReplaceAtTheEnd() {
        let s = "replace text"
        let r = o.replace(NSRange(location: 0, length: 0), s)
        XCTAssertTrue(r)
        XCTAssertEqual("replace text", o.get())
        XCTAssertEqual("replace text", o.getinput())
    }
    
    func testReplaceAtTheEndWhenHasVal() {
        let s1 = "append value 1:"
        o.append(s1)
        let s = "replace text"
        let r = o.replace(NSRange(location: s1.characters.count, length: 0), s)
        XCTAssertTrue(r)
        XCTAssertEqual("append value 1:replace text", o.get())
        XCTAssertEqual("replace text", o.getinput())
    }
    
    func testReplacePrevText() {
        let s1 = "append value 1:"
        o.append(s1)
        let s = "replace text"
        let r = o.replace(NSRange(location: 6, length: 0), s)
        XCTAssertTrue(r)
        XCTAssertEqual("appendreplace text value 1:", o.get())
        XCTAssertEqual("", o.getinput())
    }
    
    func testReplaceInputText() {
        let s1 = "output text 1:"
        o.append(s1)
        let s = "input text 1:"
        let l1 = s1.characters.count
        o.replace(NSRange(location: l1, length: 0), s)
        let s2 = "input text 2:"
        let r = o.replace(NSRange(location: l1, length: 0), s2)
        XCTAssertTrue(r)
        XCTAssertEqual("output text 1:input text 2:input text 1:", o.get())
        XCTAssertEqual("input text 2:input text 1:", o.getinput())
    }
    
    func testFixInput() {
        let s1 = "output text 1:"
        o.append(s1)
        let l1 = s1.characters.count
        let s2 = "input text 1:"
        o.replace(NSRange(location: l1, length: 0), s2)
        o.fix()
        let input = o.getinput()
        XCTAssertEqual("output text 1:input text 1:", o.get())
        XCTAssertEqual("", o.getinput())
    }
    
    var o = Termtxt()
}
