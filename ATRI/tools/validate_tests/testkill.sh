#!/bin/bash
kill -9 $(ps ax|grep 'iclingo'|grep -v 'grep'|awk '{print $1}') 1>/dev/null 2>/dev/null

