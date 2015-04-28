//
//  main.cpp
//  duplex
//
//  Created by Adel Ahmadyan on 4/23/15.
//  Copyright (c) 2015 Adel Ahmadyan. All rights reserved.
//

#include <iostream>
#include <string>
#include "configuration.h"

int main()
{
    Configuration* config = new Configuration("../config.xml");
    return 0;
}
