#! /usr/bin/env python
# -*- coding: utf-8 -*-

from lab.parser import Parser

parser = Parser()
# parser.add_pattern('num_search_generators', 'Number of search generators \(affecting facts\): (\d+)', required=False, type=int)


parser.parse()