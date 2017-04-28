#!/bin/bash
pkill m2InboxService
pkill m2ActionsServic
./m2InboxService &
./m2ActionsService &
./m2central
