#!/bin/bash
pkill m2DatabaseServi
pkill m2InboxService
pkill m2ActionsServic
./m2DatabaseService &
./m2InboxService &
./m2ActionsService &
./m2central
