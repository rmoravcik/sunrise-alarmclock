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
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.app.TimePickerDialog;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.TimePicker;
import android.util.Log;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    public static final String TAG = "MainActivity";

    private String mAddress = "20:14:10:29:10:50";

    private static final int REQUEST_ENABLE_BT = 1;

    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothClient mBluetoothClient = null;

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
                    Log.d(TAG, "BLUETOOTHCLIENT_STATE_CHANGE: " + msg.arg1);
                    break;
                case Constants.BLUETOOTHCLIENT_READ:
                    break;
                case Constants.BLUETOOTHCLIENT_TOAST:
                    Toast.makeText(context, msg.getData().getString(Constants.TOAST), Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    };


    private void setupClient() {
        Log.d(TAG, "setupClient()");

        CompoundButton.OnCheckedChangeListener onOffSwitchChanged = new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                int dayTextId = 0;
                int alarmTimeTextId = 0;

                if (buttonView.getId() == R.id.switch_day1) {
                        dayTextId = R.id.name_day1;
                        alarmTimeTextId = R.id.alarm_day1;
                } else if (buttonView.getId() == R.id.switch_day2) {
                    dayTextId = R.id.name_day2;
                    alarmTimeTextId = R.id.alarm_day2;
                } else if (buttonView.getId() == R.id.switch_day3) {
                    dayTextId = R.id.name_day3;
                    alarmTimeTextId = R.id.alarm_day3;
                } else if (buttonView.getId() == R.id.switch_day4) {
                    dayTextId = R.id.name_day4;
                    alarmTimeTextId = R.id.alarm_day4;
                } else if (buttonView.getId() == R.id.switch_day5) {
                    dayTextId = R.id.name_day5;
                    alarmTimeTextId = R.id.alarm_day5;
                } else if (buttonView.getId() == R.id.switch_day6) {
                    dayTextId = R.id.name_day6;
                    alarmTimeTextId = R.id.alarm_day6;
                } else if (buttonView.getId() == R.id.switch_day7) {
                    dayTextId = R.id.name_day7;
                    alarmTimeTextId = R.id.alarm_day7;
                }

                TextView dayText = (TextView) findViewById (dayTextId);
                TextView alarmTimeText = (TextView) findViewById (alarmTimeTextId);

                if (isChecked) {
                    dayText.setEnabled(true);
                    alarmTimeText.setEnabled(true);
                    alarmTimeText.setTextColor(getResources().getColor(android.R.color.black));
                } else {
                    dayText.setEnabled(false);
                    alarmTimeText.setEnabled(false);
                    alarmTimeText.setTextColor(dayText.getTextColors());

                    AlarmTime time = new AlarmTime(0, 0);
                    alarmTimeText.setText(time.getString());
                }
            }
        };

        ((Switch) findViewById(R.id.switch_day1)).setOnCheckedChangeListener(onOffSwitchChanged);
        ((Switch) findViewById(R.id.switch_day2)).setOnCheckedChangeListener(onOffSwitchChanged);
        ((Switch) findViewById(R.id.switch_day3)).setOnCheckedChangeListener(onOffSwitchChanged);
        ((Switch) findViewById(R.id.switch_day4)).setOnCheckedChangeListener(onOffSwitchChanged);
        ((Switch) findViewById(R.id.switch_day5)).setOnCheckedChangeListener(onOffSwitchChanged);
        ((Switch) findViewById(R.id.switch_day6)).setOnCheckedChangeListener(onOffSwitchChanged);
        ((Switch) findViewById(R.id.switch_day7)).setOnCheckedChangeListener(onOffSwitchChanged);

        mBluetoothClient = new BluetoothClient(getApplicationContext(), mHandler);

        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(mAddress);
        mBluetoothClient.connect(device);
    }

    public void layoutClicked(View view) {
        int alarmTextId = 0;
        int switchId = 0;

        if (view.getId() == R.id.layout_day1) {
            alarmTextId = R.id.alarm_day1;
            switchId = R.id.switch_day1;
        } else if (view.getId() == R.id.layout_day2) {
            alarmTextId = R.id.alarm_day2;
            switchId = R.id.switch_day2;
        } else if (view.getId() == R.id.layout_day3) {
            alarmTextId = R.id.alarm_day3;
            switchId = R.id.switch_day3;
        } else if (view.getId() == R.id.layout_day4) {
            alarmTextId = R.id.alarm_day4;
            switchId = R.id.switch_day4;
        } else if (view.getId() == R.id.layout_day5) {
            alarmTextId = R.id.alarm_day5;
            switchId = R.id.switch_day5;
        } else if (view.getId() == R.id.layout_day6) {
            alarmTextId = R.id.alarm_day6;
            switchId = R.id.switch_day6;
        } else if (view.getId() == R.id.layout_day7) {
            alarmTextId = R.id.alarm_day7;
            switchId = R.id.switch_day7;
        }

        Switch onOffSwitch = (Switch) findViewById (switchId);
        if (onOffSwitch.isChecked())
        {
            final TextView alarmText = (TextView) findViewById (alarmTextId);

            AlarmTime time = new AlarmTime(alarmText.getText().toString());
            TimePickerDialog timePicker;

            timePicker = new TimePickerDialog(MainActivity.this, new TimePickerDialog.OnTimeSetListener() {
                @Override
                public void onTimeSet(TimePicker timePicker, int selectedHour, int selectedMinute) {
                    AlarmTime time = new AlarmTime(selectedHour, selectedMinute);
                    alarmText.setText(time.getString());
                }
            }, time.getHour(), time.getMin(), true);
            timePicker.show();
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

    private class AlarmConfiguration {
        public void setAlarm(int alarm, int hour, int min)  {

        }
    }
}
