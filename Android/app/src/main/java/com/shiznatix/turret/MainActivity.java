package com.shiznatix.turret;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    public static final String TAG = Constants.TAG_PREFIX + MainActivity.class.getSimpleName();

    private final int REQUEST_ENABLE_BLUETOOTH = 1;
    private final int REQUEST_CONNECT_DEVICE = 2;

    private Toolbar mToolbar;
    private ImageButton mButtonLeft;
    private ImageButton mButtonUp;
    private ImageButton mButtonRight;
    private ImageButton mButtonDown;
    private ImageButton mButtonMiddle;
    private Button mButtonFire1;
    private Button mButtonFire2;
    private Button mButtonFire3;

    private boolean mTouchBlocked = false;
    private Handler mTouchHandler = new Handler();

    private BluetoothService mBluetoothService;
    private BluetoothAdapter mBluetoothAdapter;
    private String mConnectedDeviceName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (null == mBluetoothAdapter) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
        }

        mToolbar = (Toolbar)findViewById(R.id.toolbar);
        mToolbar.setSubtitle(getString(R.string.not_connected));
        setSupportActionBar(mToolbar);

        mButtonLeft = (ImageButton)findViewById(R.id.button_left);
        mButtonLeft.setOnTouchListener(buttonTouchListener);
        mButtonUp = (ImageButton)findViewById(R.id.button_up);
        mButtonUp.setOnTouchListener(buttonTouchListener);
        mButtonRight = (ImageButton)findViewById(R.id.button_right);
        mButtonRight.setOnTouchListener(buttonTouchListener);
        mButtonDown = (ImageButton)findViewById(R.id.button_down);
        mButtonDown.setOnTouchListener(buttonTouchListener);
        mButtonMiddle = (ImageButton)findViewById(R.id.button_middle);
        mButtonMiddle.setOnClickListener(buttonClickListener);
        mButtonFire1 = (Button)findViewById(R.id.button_fire_1);
        mButtonFire1.setOnClickListener(buttonClickListener);
        mButtonFire2 = (Button)findViewById(R.id.button_fire_2);
        mButtonFire2.setOnClickListener(buttonClickListener);
        mButtonFire3 = (Button)findViewById(R.id.button_fire_3);
        mButtonFire3.setOnClickListener(buttonClickListener);
    }

    @Override
    public void onStart() {
        super.onStart();
        // If BT is not on, request that it be enabled.
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BLUETOOTH);
            // Otherwise, setup the bluetooth session
        } else if (null == mBluetoothService) {
            setupController();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mBluetoothService != null) {
            mBluetoothService.stop();
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mBluetoothService != null) {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mBluetoothService.getState() == BluetoothService.STATE_NONE) {
                // Start the Bluetooth chat services
                mBluetoothService.start();
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_connect) {
            Intent serverIntent = new Intent(this, DeviceListActivity.class);
            startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_ENABLE_BLUETOOTH:
                if (resultCode == RESULT_OK) {
                    setupController();
                } else {
                    Toast.makeText(this, "Bluetooth must be enabled", Toast.LENGTH_LONG).show();
                    finish();
                }
                break;
            case REQUEST_CONNECT_DEVICE:
                if (resultCode == RESULT_OK) {
                    connectDevice(data);
                }
                break;
        }
    }

    private void setupController() {
        mBluetoothService = new BluetoothService(this, mHandler);
    }

    private void connectDevice(Intent data) {
        // Get the device MAC address
        String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
        Log.i(TAG, "address: " + address);
        // Get the BluetoothDevice object
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        // Attempt to connect to the device
        mBluetoothService.connect(device, false);
    }

    private void setStatus(CharSequence subTitle) {
        if (null == mToolbar) {
            return;
        }

        mToolbar.setSubtitle(subTitle);
    }

    private void sendMessage(String message) {
        Log.i(TAG, "message: " + message);

        // Check that we're actually connected before trying anything
        if (mBluetoothService.getState() != BluetoothService.STATE_CONNECTED) {
            Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT)
                 .show();
            return;
        }

        // Check that there's actually something to send
        if (message.length() > 0) {
            // Get the message bytes and tell the BluetoothChatService to write
            byte[] send = message.getBytes();
            mBluetoothService.write(send);
        }
    }

    private final View.OnClickListener buttonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            String message = null;

            switch(view.getId()) {
                case R.id.button_middle:
                    message = "m";
                    break;
                case R.id.button_fire_1:
                    message = "1";
                    break;
                case R.id.button_fire_2:
                    message = "2";
                    break;
                case R.id.button_fire_3:
                    message = "3";
                    break;
            }

            if (null != message) {
                sendMessage(message);
            }
        }
    };

    private final View.OnTouchListener buttonTouchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View view, MotionEvent motionEvent) {
            if (motionEvent.getAction() != MotionEvent.ACTION_DOWN && motionEvent.getAction() != MotionEvent.ACTION_MOVE) {
                return false;
            }

            String message = null;

            switch (view.getId()) {
                case R.id.button_left:
                    message = "l";
                    break;
                case R.id.button_up:
                    message = "u";
                    break;
                case R.id.button_right:
                    message = "r";
                    break;
                case R.id.button_down:
                    message = "d";
                    break;
            }

            if (null != message) {
                if (!mTouchBlocked) {
                    mTouchBlocked = true;
                    sendMessage(message);

                    mTouchHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            mTouchBlocked = false;
                        }
                    }, 100);
                } else {
                    return false;
                }

                return true;
            }

            return false;
        }
    };

    /**
     * The Handler that gets information back from the BluetoothChatService
     */
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Constants.MESSAGE_STATE_CHANGE:
                    switch (msg.arg1) {
                        case BluetoothService.STATE_CONNECTED:
                            setStatus(getString(R.string.title_connected_to, mConnectedDeviceName));
                            break;
                        case BluetoothService.STATE_CONNECTING:
                            setStatus(getString(R.string.title_connecting));
                            break;
                        case BluetoothService.STATE_LISTEN:
                        case BluetoothService.STATE_NONE:
                            setStatus(getString(R.string.title_not_connected));
                            break;
                    }
                    break;
                case Constants.MESSAGE_WRITE:
                    byte[] writeBuf = (byte[]) msg.obj;
                    // construct a string from the buffer
                    String writeMessage = new String(writeBuf);

                    Log.d(TAG, "Write message: " + writeMessage);
                    break;
                case Constants.MESSAGE_READ:
                    byte[] readBuf = (byte[]) msg.obj;
                    // construct a string from the valid bytes in the buffer
                    String readMessage = new String(readBuf, 0, msg.arg1);
                    Log.d(TAG, mConnectedDeviceName + ":  " + readMessage);
                    break;
                case Constants.MESSAGE_DEVICE_NAME:
                    // save the connected device's name
                    mConnectedDeviceName = msg.getData().getString(Constants.DEVICE_NAME);

                    Toast.makeText(MainActivity.this, "Connected to " + mConnectedDeviceName, Toast.LENGTH_SHORT)
                         .show();
                    break;
                case Constants.MESSAGE_TOAST:
                    Toast.makeText(MainActivity.this, msg.getData().getString(Constants.TOAST), Toast.LENGTH_SHORT)
                         .show();
                    break;
            }
        }
    };
}
