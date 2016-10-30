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

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.UUID;

public class BluetoothClient {
    private static final String TAG = "BluetoothClient";

    private static final UUID MY_UUID =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    public static final int STATE_NONE = 0;       // we're doing nothing
    public static final int STATE_CONNECTING = 1; // now initiating an outgoing connection
    public static final int STATE_CONNECTED = 2;  // now connected to a remote device

    private ConnectThread mConnectThread;
    private ConnectedThread mConnectedThread;

    private int mState;

    private final Context mContext;
    private final BluetoothAdapter mAdapter;
    private final Handler mHandler;

    public BluetoothClient(Context context, Handler handler) {
        mContext = context;
        mAdapter = BluetoothAdapter.getDefaultAdapter();
        mState = STATE_NONE;
        mHandler = handler;
    }

    private synchronized void setState(int state) {
        Log.d(TAG, "setState " + mState + " -> " + state);

        mState = state;

        mHandler.obtainMessage(Constants.BLUETOOTHCLIENT_STATE_CHANGE, state, -1).sendToTarget();
    }

    public synchronized int getState() {
        return mState;
    }

    public synchronized void stop() {
        Log.d(TAG, "stop");

        if (mConnectThread != null) {
            mConnectThread.cancel();
            mConnectThread = null;
        }

        if (mConnectedThread != null) {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        setState(STATE_NONE);
    }

    public synchronized void connect(BluetoothDevice device) {
        Log.d(TAG, "connecting to: " + device);

        if (mState == STATE_CONNECTING) {
            if (mConnectThread != null) {
                mConnectThread.cancel();
                mConnectThread = null;
            }
        }

        if (mConnectedThread != null) {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        mConnectThread = new ConnectThread(device);
        mConnectThread.start();

        setState(STATE_CONNECTING);
    }

    public synchronized void connected(BluetoothSocket socket, BluetoothDevice
            device) {
        Log.d(TAG, "connected...");

        if (mConnectThread != null) {
            mConnectThread.cancel();
            mConnectThread = null;
        }

        if (mConnectedThread != null) {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        mConnectedThread = new ConnectedThread(socket);
        mConnectedThread.start();
    }

     private void connectionFailed() {
        Log.d(TAG, "connection failed...");

        Message msg = mHandler.obtainMessage(Constants.BLUETOOTHCLIENT_TOAST);
        Bundle bundle = new Bundle();
        bundle.putString(Constants.TOAST, mContext.getString(R.string.unable_connect_device));
        msg.setData(bundle);
        mHandler.sendMessage(msg);

        BluetoothClient.this.stop();
    }

    private void connectionLost() {
        Log.d(TAG, "connection lost...");

        Message msg = mHandler.obtainMessage(Constants.BLUETOOTHCLIENT_TOAST);
        Bundle bundle = new Bundle();
        bundle.putString(Constants.TOAST, mContext.getString(R.string.connection_lost));
        msg.setData(bundle);
        mHandler.sendMessage(msg);

        BluetoothClient.this.stop();
    }

    public synchronized void write(String out) {
        Log.d(TAG, "write: " + out);

        ConnectedThread r;

        synchronized (this) {
            if (mState != STATE_CONNECTED) return;
            r = mConnectedThread;
        }

        r.write(out);
    }

    private class ConnectThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;

        public ConnectThread(BluetoothDevice device) {
            mmDevice = device;
            BluetoothSocket tmp = null;

            try {
                tmp = device.createInsecureRfcommSocketToServiceRecord(MY_UUID);
            } catch (IOException e) {
                Log.e(TAG, "create socket failed...", e);
            }
            mmSocket = tmp;
        }

        public void run() {
            Log.i(TAG, " ConnectThread.run()");
            setName("ConnectThread");

            mAdapter.cancelDiscovery();

            try {
                mmSocket.connect();
            } catch (IOException e) {
                try {
                    mmSocket.close();
                } catch (IOException e2) {
                    Log.e(TAG, "unable to close socket...", e2);
                }
                connectionFailed();
                return;
            }

            synchronized (BluetoothClient.this) {
                mConnectThread = null;
            }

            connected(mmSocket, mmDevice);
        }

        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "unable to close socket...", e);
            }
        }
    }

    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final BufferedReader mBufferedReader;
        private final BufferedWriter mBufferedWriter;

        public ConnectedThread(BluetoothSocket socket) {
            Log.d(TAG, "creating ConnectedThread...");
            setName("ConnectedThread");

            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Log.e(TAG, "failed to get socket streams", e);
            }

            mBufferedReader = new BufferedReader(new InputStreamReader(tmpIn));
            mBufferedWriter = new BufferedWriter(new OutputStreamWriter(tmpOut));
        }

        public void run() {
            Log.i(TAG, "ConnectedThread.run()");

            setState(STATE_CONNECTED);

            while (mState == STATE_CONNECTED) {
                try {
                    String response = mBufferedReader.readLine();
                    Log.d(TAG, "recevied: " + response);

                    Message msg = mHandler.obtainMessage(Constants.BLUETOOTHCLIENT_READ);
                    Bundle bundle = new Bundle();
                    bundle.putString(Constants.READ, response);
                    msg.setData(bundle);
                    mHandler.sendMessage(msg);
                } catch (IOException e) {
                    Log.e(TAG, "disconnected", e);
                    connectionLost();
                    break;
                }
            }
        }

        public void write(String request) {
            try {
                mBufferedWriter.write(request);
                mBufferedWriter.flush();
            } catch (IOException e) {
                Log.e(TAG, "failed to write to socket", e);
            }
        }

        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "failed to close socket", e);
            }
        }
    }
}
