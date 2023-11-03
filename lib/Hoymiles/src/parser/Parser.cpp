// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022 - 2023 Thomas Basler and others
 */
#include "Parser.h"

Parser::Parser()
{

}

uint32_t Parser::getLastUpdate()
{
    return _lastUpdate;
}

void Parser::setLastUpdate(uint32_t lastUpdate)
{
    _lastUpdate = lastUpdate;
}

void Parser::beginAppendFragment()
{

}

void Parser::endAppendFragment()
{
    
}