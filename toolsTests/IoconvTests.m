//
//  IoconvTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/22.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "decl.h"
#include <sh.h>
#import <XCTest/XCTest.h>
#include <string.h>

@interface IoconvTests : XCTestCase
{
FILE *rfp;
int rfd;
FILE *fps[4][2];
char b[256];
}
@end

@implementation IoconvTests

- (void)setUp {
    size_t l;
    [super setUp];
    rfp = 0;
    rfd = -1;
    mktestdir("IoconvTests", b, sizeof b);
    l = strlen(b);
#undef OPEN
#define OPEN(n, i, j) strcpy(b + l, n); fps[i][j] = fopen(b, "w+b");
    OPEN("f00", 0, 0)
    OPEN("f01", 0, 1)
    OPEN("f10", 1, 0)
    OPEN("f11", 1, 1)
    OPEN("f20", 2, 0)
    OPEN("f21", 2, 1)
    OPEN("f30", 3, 0)
    fps[3][1] = 0;
}

- (void)tearDown {
    int i;
    FILE **f = fps[0];
    for (i = 0; i < sizeof fps / sizeof fps[0][0]; ++i)
        if (f[i])
            fclose(f[i]);
    [super tearDown];
}

- (void)testDoesntContainTarget {
    rfp = convfp(fps, fps[3][0]);
    XCTAssertEqual(fps[3][0], rfp);
}

- (void)test1stEntryIsTarget {
    rfp = convfp(fps, fps[0][0]);
    XCTAssertEqual(fps[0][1], rfp);
}

- (void)testLastEntryIsTarget {
    rfp = convfp(fps, fps[2][0]);
    XCTAssertEqual(fps[2][1], rfp);
}

- (void)testDoesntContainTargetFd {
    rfd = convfd(fps, fileno(fps[3][0]));
    XCTAssertEqual(fileno(fps[3][0]), rfd);
}

- (void)test1stEntryIsTargetFd {
    rfd = convfd(fps, fileno(fps[0][0]));
    XCTAssertEqual(fileno(fps[0][1]), rfd);
}

- (void)testLastEntryIsTargetFd {
    rfd = convfd(fps, fileno(fps[2][0]));
    XCTAssertEqual(fileno(fps[2][1]), rfd);
}

@end
