//
//  fwrap.m
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/04.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#import <Foundation/Foundation.h>

NSURL
*getdocdir()
{
    NSFileManager *m = [NSFileManager defaultManager];
    return [[m URLsForDirectory:NSDocumentDirectory
                      inDomains:NSUserDomainMask]firstObject];
}

NSURL
*getlibdir()
{
    NSFileManager *m = [NSFileManager defaultManager];
    return [[m URLsForDirectory:NSLibraryDirectory
                      inDomains:NSUserDomainMask]firstObject];
}