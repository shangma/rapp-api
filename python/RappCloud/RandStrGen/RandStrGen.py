#!/usr/bin/env python
# -*- coding: utf-8 -*-


# Copyright 2015 RAPP

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

    #http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Authors: Konstantinos Panayiotou, Manos Tsardoulias
# contact: klpanagi@gmail.com, etsardou@iti.gr


## @file RandStrGen/RandStrGen.py
#
#  @copyright Rapp Projecty EU 2015
#  @author Konstantinos Panayiotou, [klpanagi@gmail.com]
#


import random


## @class RandStrGen
#
#  Random String Generator class. Generate random strings. Static class.
#
class RandStrGen:
    # @var __charsArray Characters Array.
    __charsArray = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    # @var __defaultSize Default generated string size.
    __defaultSize = 5


    ## Generate a new random string.
    #
    #  @param size String size.
    #
    @staticmethod
    def create(size):
        randStr = ''
        for i in range(size):
            # Random number in range of __charsArray size.
            randNum = random.randint(0, len(RandStrGen.__charsArray) -1)
            randStr += RandStrGen.__charsArray[randNum:randNum + 1]
        return randStr
