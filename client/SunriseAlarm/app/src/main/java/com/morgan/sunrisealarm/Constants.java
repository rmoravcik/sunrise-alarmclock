/*
 * Copyright (C) 2016 Roman Moravcik
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Based on code of: Android BluetoothChat Sample
 *
 */

package com.morgan.sunrisealarm;

public interface Constants {
    public static final int ALARM1 = 1;
    public static final int ALARM2 = 2;
    public static final int ALARM3 = 3;
    public static final int ALARM4 = 4;
    public static final int ALARM5 = 5;
    public static final int ALARM6 = 6;
    public static final int ALARM7 = 7;

    public static final int HOUR_ALARM_OFF = 255;
    public static final int MIN_ALARM_OFF = 255;

    public static final int BLUETOOTHCLIENT_STATE_CHANGE = 1;
    public static final int BLUETOOTHCLIENT_READ = 2;
    public static final int BLUETOOTHCLIENT_TOAST = 3;

    public static final String TOAST = "toast";

    public static final String COMMAND_GET_STATUS = "STAT?";
    public static final String COMMAND_GET_STATUS_RSP = "STAT+";
    public static final String COMMAND_SET_ALARM = "ALARM+";
    public static final String COMMAND_SET_ALARM_RSP = "ALARM+OK";
    public static final String COMMAND_SET_DATE = "DATE+";
    public static final String COMMAND_SET_DATE_RSP = "DATE+OK";
}
