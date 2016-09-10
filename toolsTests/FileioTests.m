//
//  FileioTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/08.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "decl.h"
#import <XCTest/XCTest.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

@interface FileioTests : XCTestCase
{
    int initok;
    char tdir[256];
}

@end

@implementation FileioTests

- (void)setUp {
    [super setUp];
    if (mktestdir("FileioTests", tdir, sizeof tdir) == 0)
        initok = 1;
    else
        initok = 0;
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testReplaceStdin {
    int r;
    FILE *f;
    char b[256];
    
    XCTAssertTrue(initok);
    XCTAssertTrue(strlen(tdir) + 32 < 256);
    strcpy(b, tdir);
    strcat(b, "/in");
    r = replacein(b, &f);
    XCTAssertEqual(0, r);
    XCTAssertEqual(stdin, f);
}

@end
