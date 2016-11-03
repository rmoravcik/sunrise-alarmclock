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

import android.app.Activity;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.app.TimePickerDialog;
import android.text.format.DateFormat;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.TimePicker;
import android.util.Log;
import android.widget.Toast;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

public class MainActivity extends AppCompatActivity {

    public static final String TAG = "MainActivity";

    private String mAddress = "20:14:10:29:10:50";

    private static final int REQUEST_ENABLE_BT = 1;

    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothClient mBluetoothClient = null;

    private static boolean mWaitingAck = false;
    private static int mWaitingAckId = -1;

    CompoundButton.OnCheckedChangeListener mOnOffSwitchChanged;

    ProgressDialog mDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, getString(R.string.bluetooth_not_available), Toast.LENGTH_LONG).show();
            this.finish();
        }
    }

    @Override
    public void onStart() {
        super.onStart();
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        } else if (mBluetoothClient == null) {
            setupClient();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mBluetoothClient != null) {
            mBluetoothClient.stop();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_ENABLE_BT:
                if (resultCode == Activity.RESULT_OK) {
                    Log.d(TAG, "Bluetooth enabled");
                    setupClient();
                } else {
                    Log.e(TAG, "Bluetooth not enabled");
                    this.finish();
                }
                break;
        }
    };

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Context context = getApplicationContext();

            switch (msg.what) {
                case Constants.BLUETOOTHCLIENT_STATE_CHANGE:
                    switch (msg.arg1) {
                        case BluetoothClient.STATE_NONE:
                            break;

                        case BluetoothClient.STATE_CONNECTING:
                            mDialog = new ProgressDialog(MainActivity.this);
                            mDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
                            mDialog.setMessage(getString(R.string.connecting_please_wait));
                            mDialog.setIndeterminate(true);
                            mDialog.setCanceledOnTouchOutside(false);
                            mDialog.show();
                            break;

                        case BluetoothClient.STATE_CONNECTED:
                            Command command = new Command();
                            command.ping();
                            break;

                        default:
                    }
                    break;
                case Constants.BLUETOOTHCLIENT_READ:
                    String response = msg.getData().getString(Constants.READ);

                    Command command = new Command(response);
                    break;
                case Constants.BLUETOOTHCLIENT_WRITE:
                    if (mWaitingAck) {
                        Command command_retry = new Command();
                        switch (mWaitingAckId) {
                            case Constants.COMMAND_PING_RSP_ID:
                                command_retry.ping();
                                break;
                            case Constants.COMMAND_GET_STATUS_RSP_ID:
                                command_retry.getStatus();
                                break;
                            default:
                        }
                    }
                    break;
                case Constants.BLUETOOTHCLIENT_TOAST:
                    Toast.makeText(context, msg.getData().getString(Constants.TOAST), Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    };

    private void setupClient() {
        Log.d(TAG, "setupClient()");

        mOnOffSwitchChanged = new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                WidgetId widget = new WidgetId(buttonView);

                Log.d(TAG, "OnCheckedChangeListener");

                TextView dayText = (TextView) findViewById (widget.getDayTextId());
                TextView alarmTimeText = (TextView) findViewById (widget.getAlarmTextId());

                if (isChecked) {
                    dayText.setEnabled(true);
                    alarmTimeText.setEnabled(true);
                    alarmTimeText.setTextColor(getResources().getColor(android.R.color.black));

                    AlarmTime time = new AlarmTime(alarmTimeText.getText().toString());

                    Command command = new Command();
                    command.setAlarm(widget.getAlarmId(), time.getHour(), time.getHour());
                } else {
                    dayText.setEnabled(false);
                    alarmTimeText.setEnabled(false);
                    alarmTimeText.setTextColor(dayText.getTextColors());

                    AlarmTime time = new AlarmTime(0, 0);
                    alarmTimeText.setText(time.getString());

                    Command command = new Command();
                    command.setAlarm(widget.getAlarmId(), Constants.HOUR_ALARM_OFF, Constants.MIN_ALARM_OFF);
                }
            }
        };

        mBluetoothClient = new BluetoothClient(getApplicationContext(), mHandler);

        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(mAddress);
        mBluetoothClient.connect(device);
    }

    public void layoutClicked(View view) {
        final WidgetId widget = new WidgetId(view);

        Switch onOffSwitch = (Switch) findViewById (widget.getSwitchId());
        if (onOffSwitch.isChecked())
        {
            final TextView alarmText = (TextView) findViewById (widget.getAlarmTextId());

            AlarmTime time = new AlarmTime(alarmText.getText().toString());
            TimePickerDialog timePicker;

            timePicker = new TimePickerDialog(MainActivity.this, new TimePickerDialog.OnTimeSetListener() {
                @Override
                public void onTimeSet(TimePicker timePicker, int selectedHour, int selectedMinute) {
                    AlarmTime time = new AlarmTime(selectedHour, selectedMinute);
                    alarmText.setText(time.getString());

                    Command command = new Command();
                    command.setAlarm(widget.getAlarmId(), selectedHour, selectedMinute);
                }
            }, time.getHour(), time.getMin(), true);
            timePicker.show();
        }
    }

private class WidgetId {
    private int mAlarmId;
    private int mAlarmTextId;
    private int mDayTextId;
    private int mSwitchId;

    public WidgetId(View view) {
        if ((view.getId() == R.id.layout_day1) ||
                (view.getId() == R.id.switch_day1)) {
            mAlarmId = Constants.ALARM1;
        } else if ((view.getId() == R.id.layout_day2) ||
                (view.getId() == R.id.switch_day2)) {
            mAlarmId = Constants.ALARM2;
        } else if ((view.getId() == R.id.layout_day3) ||
                (view.getId() == R.id.switch_day3)) {
            mAlarmId = Constants.ALARM3;
        } else if ((view.getId() == R.id.layout_day4) ||
                (view.getId() == R.id.switch_day4)) {
            mAlarmId = Constants.ALARM4;
        } else if ((view.getId() == R.id.layout_day5) ||
                (view.getId() == R.id.switch_day5)) {
            mAlarmId = Constants.ALARM5;
        } else if ((view.getId() == R.id.layout_day6) ||
                (view.getId() == R.id.switch_day6)) {
            mAlarmId = Constants.ALARM6;
        } else if ((view.getId() == R.id.layout_day7) ||
                (view.getId() == R.id.switch_day7)) {
            mAlarmId = Constants.ALARM7;
        }
        init();
    }

    public WidgetId(int alarmId) {
        mAlarmId = alarmId;
        init();
    }

    private void init() {
        if (mAlarmId == Constants.ALARM1) {
            mAlarmTextId = R.id.alarm_day1;
            mDayTextId = R.id.name_day1;
            mSwitchId = R.id.switch_day1;
        } else if (mAlarmId == Constants.ALARM2) {
            mAlarmTextId = R.id.alarm_day2;
            mDayTextId = R.id.name_day2;
            mSwitchId = R.id.switch_day2;
        } else if (mAlarmId == Constants.ALARM3) {
            mAlarmTextId = R.id.alarm_day3;
            mDayTextId = R.id.name_day3;
            mSwitchId = R.id.switch_day3;
        } else if (mAlarmId == Constants.ALARM4) {
            mAlarmTextId = R.id.alarm_day4;
            mDayTextId = R.id.name_day4;
            mSwitchId = R.id.switch_day4;
        } else if (mAlarmId == Constants.ALARM5) {
            mAlarmTextId = R.id.alarm_day5;
            mDayTextId = R.id.name_day5;
            mSwitchId = R.id.switch_day5;
        } else if (mAlarmId == Constants.ALARM6) {
            mAlarmTextId = R.id.alarm_day6;
            mDayTextId = R.id.name_day6;
            mSwitchId = R.id.switch_day6;
        } else if (mAlarmId == Constants.ALARM7) {
            mAlarmTextId = R.id.alarm_day7;
            mDayTextId = R.id.name_day7;
            mSwitchId = R.id.switch_day7;
        }
    }

    int getAlarmId() {
        return mAlarmId;
    }

    int getAlarmTextId() {
        return mAlarmTextId;
    }

    int getDayTextId() {
        return mDayTextId;
    }

    int getSwitchId() {
        return mSwitchId;
    }
}

private class AlarmTime {
        private int mHour = 0;
        private int mMin = 0;

        public AlarmTime(int hour, int min) {
            mHour = hour;
            mMin = min;
        }

        public AlarmTime(String time) {
            String list[] = time.split(":", 2);
            mHour = Integer.parseInt(list[0]);
            mMin = Integer.parseInt(list[1]);
        }

        public int getHour() {
            return mHour;
        }

        public int getMin() {
            return mMin;
        }

        public String getString() {
            return String.format("%02d:%02d", mHour, mMin);
        }
    }

    private class Command {
        public Command() {}

        public Command(String response)
        {
            if (response.contains(Constants.COMMAND_PING_RSP)) {
                Log.i(TAG, "received COMMAND_PING_RSP");
                mWaitingAck = false;
                mWaitingAckId = -1;
                getStatus();
            } else if (response.contains(Constants.COMMAND_GET_STATUS_RSP)) {
                Log.i(TAG, "received COMMAND_GET_STATUS_RSP");
                String tmp = response.replace(Constants.COMMAND_GET_STATUS_RSP, "");
                String[] alarmList = tmp.split(";");

                if (alarmList.length == Constants.ALARM_MAX) {
                    mWaitingAck = false;
                    mWaitingAckId = -1;

                    for (int alarmId = Constants.ALARM1;
                         alarmId < Constants.ALARM_MAX; alarmId++) {
                        WidgetId widget = new WidgetId(alarmId);

                        Switch onOffSwitch =
                                (Switch) findViewById(widget.getSwitchId());

                        onOffSwitch.setEnabled(true);

                        if (!alarmList[alarmId].equals(Constants.ALARM_OFF)) {
                            TextView dayText =
                                    (TextView) findViewById (widget.getDayTextId());
                            TextView alarmTimeText =
                                    (TextView) findViewById (widget.getAlarmTextId());

                            dayText.setEnabled(true);
                            alarmTimeText.setEnabled(true);
                            alarmTimeText.setTextColor(ContextCompat.getColor(
                                    getApplicationContext(), android.R.color.black));
                            alarmTimeText.setText(alarmList[alarmId]);
                            onOffSwitch.setChecked(true);
                        }
                        onOffSwitch.setOnCheckedChangeListener(mOnOffSwitchChanged);
                    }
                    mDialog.dismiss();
                    setDate();
                }
            } else if (response.contains(Constants.COMMAND_SET_DATE_RSP)) {
                Log.i(TAG, "received COMMAND_SET_DATE_RSP");
                mWaitingAck = false;
                mWaitingAckId = -1;
            }
        }

        public void ping() {
            String command = Constants.COMMAND_PING + "\n";
            mWaitingAck = true;
            mWaitingAckId = Constants.COMMAND_PING_RSP_ID;
            mBluetoothClient.write(command);
        }

        public void getStatus() {
            String command = Constants.COMMAND_GET_STATUS + "\n";
            mWaitingAck = true;
            mWaitingAckId = Constants.COMMAND_GET_STATUS_RSP_ID;
            mBluetoothClient.write(command);
        }

        public void setAlarm(int alarm, int hour, int min)  {
            String command = Constants.COMMAND_SET_ALARM +
                    String.format("%d;%02d:%02d\n", alarm, hour, min);
            mWaitingAck = true;
            mBluetoothClient.write(command);
        }

        public void setDate()  {
            Calendar calendar = Calendar.getInstance();

            String wday;
            switch (calendar.get(Calendar.DAY_OF_WEEK)) {
                case Calendar.MONDAY:
                    wday = ";1;";
                    break;
                case Calendar.TUESDAY:
                    wday = ";2;";
                    break;
                case Calendar.WEDNESDAY:
                    wday = ";3;";
                    break;
                case Calendar.THURSDAY:
                    wday = ";4;";
                    break;
                case Calendar.FRIDAY:
                    wday = ";5;";
                    break;
                case Calendar.SATURDAY:
                    wday = ";6;";
                    break;
                case Calendar.SUNDAY:
                    wday = ";7;";
                    break;
                default:
                    wday = ";0;";
            }

            SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss;;dd.MM.yyyy");

            String command = Constants.COMMAND_SET_DATE +
                    dateFormat.format(calendar.getTime()).toString()
                            .replace(";;", wday) + "\n";

            mWaitingAck = true;
            mBluetoothClient.write(command);
        }
    }
}
