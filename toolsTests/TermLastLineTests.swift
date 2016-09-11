//
//  TermLastLineTests.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/11.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import XCTest

class TermLastLineTests: XCTestCase {

    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }

    func testWithEmpty() {
        let r = getlastline("")
        XCTAssertNil(r)
    }
    
    func testWithNL() {
        let r = getlastline("\n")
        XCTAssertEqual("", r)
    }
    
    func testPSwoNL() {
        let r = getlastline("$ ")
        XCTAssertNil(r)
    }
    
    func testPSwNL() {
        let r = getlastline("$ \n")
        XCTAssertEqual("", r)
    }
    
    func testPSwS() {
        let r = getlastline("$ abcdefg\n")
        XCTAssertEqual("abcdefg", r)
    }
    
    func testPSwSwoNL() {
        let r = getlastline("$ abcdefg")
        XCTAssertNil(r)
    }
    
    func testMulti() {
        let r = getlastline("$ abcdefg\n$ hijklmn\n")
        XCTAssertEqual("hijklmn", r)
    }
}
