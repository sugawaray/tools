//
//  ParserGrammerTests.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/18.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <sh.h>
#import <XCTest/XCTest.h>

@interface ParserGrammerTests : XCTestCase
{
struct Wl *wl;
}

@end

@implementation ParserGrammerTests

- (void)setUp {
    [super setUp];
    initsh();
}

- (void)tearDown {
    freeshwl();
    cleanupsh();
    [super tearDown];
}

- (void)testSingleCommandInLine {
    [self procInput:"cmd a b\n"];
    
    wl = shwltail->next;
    XCTAssertEqual(0, strcmp(wl->p, "cmd"));
    XCTAssertEqual(0, strcmp(wl->next->p, "a"));
    XCTAssertEqual(0, strcmp(wl->next->next->p, "b"));
}

- (void)testSimpleCommandWithRedirect {
    [self procInput:"cmd a b >c\n"];
    
    wl = shwltail->next;
    XCTAssertEqual(0, strcmp(wl->p, "cmd"));
    XCTAssertEqual(0, strcmp(wl->next->p, "a"));
    XCTAssertEqual(0, strcmp(wl->next->next->p, "b"));
    XCTAssertEqual(0, strcmp(redirp, "c"));
}

- (void)procInput:(const char *)v {
    putsonin(v);
    clearerr(stdin);
    yyparse();
}

@end
