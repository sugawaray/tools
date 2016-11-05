//
//  CopyTestDataTests.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/11/05.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import XCTest

class CopyTestDataTests: XCTestCase {
    
    override func setUp() {
        super.setUp()
    }
    
    override func tearDown() {
        super.tearDown()
    }
    
    func test() {
        copyToDocDir(name:"abc", ofType:"txt", inDirectory:"CmdCallTestsTestd")
        let fm = FileManager.default
        var doc:[URL] = fm.urls(for: FileManager.SearchPathDirectory.documentDirectory, in: FileManager.SearchPathDomainMask.userDomainMask)
        doc[0].appendPathComponent("abc.txt")
        XCTAssertTrue(fm.fileExists(atPath: doc[0].path))
    }

}
