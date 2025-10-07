package com.nanofuxion.xinputbridge

import android.os.IBinder
import android.view.View
import android.app.Service
import android.content.Intent
import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.os.Build

class OverlayService : Service() {

    private var overlayView: View? = null
    private var initialX = 0
    private var initialY = 0
    private var run = false

    companion object {
        private const val NOTIFICATION_ID = 1
        private const val CHANNEL_ID = "XinputBridgeChannel"
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        if (!run) {
            run = true

            createNotificationChannel()
            val notification = Notification.Builder(this, CHANNEL_ID)
                .setContentTitle("Xinput Bridge")
                .setContentText("Service is running to bridge gamepad input.")
                .setSmallIcon(R.mipmap.ic_launcher)
                .build()

            startForeground(NOTIFICATION_ID, notification)

            showOverlay()
            println("I! Start Overlay")
        } else {
            println("E! Overlay running")
        }

        return START_NOT_STICKY
    }

    override fun onDestroy() {
        if (run) {
            run = false
            stopForeground(true)
            super.onDestroy()
            removeOverlay()
            println("I! Overlay Stopped")
        } else {
            println("E! Overlay not running")
        }
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val serviceChannel = NotificationChannel(
                CHANNEL_ID,
                "Xinput Bridge Service Channel",
                NotificationManager.IMPORTANCE_LOW
            )
            val manager = getSystemService(NotificationManager::class.java)
            manager.createNotificationChannel(serviceChannel)
        }
    }

    private fun showOverlay() {
        OverlayManager.showOverlay(this, initialX, initialY)
    }

    private fun removeOverlay() {
        OverlayManager.removeOverlay(this)
    }
}
