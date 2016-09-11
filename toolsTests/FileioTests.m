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

- (void)testMktestdir {
    FILE *f;
    XCTAssertEqual(1, initok);
    XCTAssertTrue(strlen(tdir) < sizeof tdir - 32);
    strcat(tdir, "/somefile");
    f = fopen(tdir, "w+b");
    XCTAssertTrue(f != 0);
    fclose(f);
}

@end
