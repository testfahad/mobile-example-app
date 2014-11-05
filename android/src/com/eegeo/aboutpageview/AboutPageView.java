//Copyright eeGeo Ltd (2012-2014), All Rights Reserved

package com.eegeo.aboutpageview;

import android.view.View;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.eegeo.mobileexampleapp.MainActivity;
import com.eegeo.mobileexampleapp.R;

public class AboutPageView implements View.OnClickListener
{
	protected MainActivity m_activity = null;
	protected long m_nativeCallerPointer;
	private View m_view = null;
	private RelativeLayout m_uiRoot = null;
	private View m_closeButton = null;
	private TextView m_aboutTextView = null;
	private TextView m_platformVersionView = null;
	private TextView m_platformHashView = null;
 
	public AboutPageView(MainActivity activity, long nativeCallerPointer)
	{
		m_activity = activity;
		m_nativeCallerPointer = nativeCallerPointer;
		
		final AboutPageView self = this;
		
		m_activity.runOnUiThread(new Runnable()
		{
			public void run()
			{
				m_uiRoot = (RelativeLayout)m_activity.findViewById(R.id.ui_container);
				
				m_view = m_activity.getLayoutInflater().inflate(R.layout.about_page_layout, m_uiRoot, false);
				m_closeButton = m_view.findViewById(R.id.about_page_view_close_button);
				m_aboutTextView = (TextView)m_view.findViewById(R.id.about_page_view_about_text);
				m_platformVersionView = (TextView)m_view.findViewById(R.id.about_page_view_platform_version);
				m_platformHashView = (TextView)m_view.findViewById(R.id.about_page_view_platform_hash);
				
				m_closeButton.setOnClickListener(self);
				m_view.setVisibility(View.GONE);
				m_uiRoot.addView(m_view);
			}
		});
	}
	
	public void destroy()
	{
		m_activity.runOnUiThread(new Runnable()
		{
			public void run()
			{
				m_uiRoot.removeView(m_view);
			}
		});
	}

	public void displayPoiInfo(final String aboutText, final String platformHash, final String platformVersion)
	{
		m_activity.runOnUiThread(new Runnable()
		{
			public void run()
			{
				m_aboutTextView.setText(aboutText);
				m_platformVersionView.setText("Platform version: " + platformVersion);
				m_platformHashView.setText("Platform hash: " + platformHash);

				m_closeButton.setEnabled(true);
				m_view.setVisibility(View.VISIBLE);
				m_view.requestFocus();
			}
		});	
	}
	
	public void dismissAboutPage()
	{
		m_activity.runOnUiThread(new Runnable()
		{
			public void run()
			{
				m_view.setVisibility(View.GONE);
			}
		});	
	}
	
	public void onClick(View view)
	{
		m_closeButton.setEnabled(false);
		m_activity.runOnNativeThread(new Runnable()
		{
			public void run()
			{
				AboutPageViewJniMethods.CloseButtonClicked(m_nativeCallerPointer);
			}
		});
	}	
}