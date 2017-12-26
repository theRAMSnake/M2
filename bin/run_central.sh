#!/bin/bash
pkill m2DatabaseServi
pkill m2InboxService
pkill m2ActionsServic
pkill m2Calendar
./m2DatabaseService &
./m2InboxService &
./m2ActionsService &
./m2Calendar &
./m2central
