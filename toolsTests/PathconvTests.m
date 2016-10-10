//
//  PathconvTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/08.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <path.h>
#include <sh.h>
#import <XCTest/XCTest.h>

@interface PathconvTests : XCTestCase {

int r1;
char b[512];

}

@end

@implementation PathconvTests

- (void)setUp {
    [super setUp];
    initsh();
}

- (void)tearDown {
    cleanupsh();
    [super tearDown];
}

#define CMPNTAIL(s, b)  (strcmp((s), &(b)[strlen((b)) - strlen((s))]))

- (void)testRoot {
    r1 = convpath("/", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/"));
    XCTAssertTrue(CMPNTAIL("/Documents/", b) == 0);
}

- (void)testFileAtRoot {
    r1 = convpath("/afile", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/afile"));
    XCTAssertTrue(CMPNTAIL("/Documents/afile", b) == 0);
}

- (void)testDir {
    r1 = convpath("/adir/", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/adir/"));
    XCTAssertTrue(CMPNTAIL("/Documents/adir/", b) == 0);
}

- (void)testCurrent {
    r1 = convpath(".", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/"));
    XCTAssertTrue(CMPNTAIL("/Documents/", b) == 0);
}

- (void)testParent {
    r1 = convpath("..", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/"));
    XCTAssertTrue(CMPNTAIL("/Documents/", b) == 0);
}

- (void)testCleanupSuccessiveSlash {
    r1 = convpath("//d1//d2///d3//", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/d1/d2/d3/"));
    XCTAssertTrue(CMPNTAIL("/Documents/d1/d2/d3/", b) == 0);
}

- (void)testInterpretCurrent {
    r1 = convpath("/d1/./d2/./d3/.", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/d1/d2/d3/"));
    XCTAssertTrue(CMPNTAIL("/Documents/d1/d2/d3/", b) == 0);
}

- (void)testInterpretParent {
    r1 = convpath("/d1/../d2/../d3/..", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/"));
    XCTAssertTrue(CMPNTAIL("/Documents/", b) == 0);
}

- (void)testParentAtRoot {
    r1 = convpath("/..", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/"));
    XCTAssertTrue(CMPNTAIL("/Documents/", b) == 0);
}

- (void)testParentAtRootSlashEnd {
    r1 = convpath("/../", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/"));
    XCTAssertTrue(CMPNTAIL("/Documents/", b) == 0);
}

- (void)testEmpty {
    r1 = convpath("", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/"));
    XCTAssertTrue(CMPNTAIL("/Documents/", b) == 0);
}

- (void)test1stNonSlash {
    r1 = convpath("d1", b, sizeof b);
    XCTAssertEqual(0, r1);
    XCTAssertTrue(strlen(b) > strlen("/Documents/d1"));
    XCTAssertTrue(CMPNTAIL("/Documents/d1", b) == 0);
}

@end
