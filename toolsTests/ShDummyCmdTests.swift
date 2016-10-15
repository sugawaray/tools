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
        super.tearDown()
    }
    
    func testReadStdin() {
        o.proc(Sh.dummycmd)
        putsonin("input")
        var s: String? = nil
        while (true) {
            s = o.getdummycmdinput()
            if (s != nil && s!.characters.count > 0) {
                break
            }
        }
        XCTAssertEqual("input", s)
    }
    
    var o = Sh()
}
