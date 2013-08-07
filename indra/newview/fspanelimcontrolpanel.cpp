/** 
 * @file fspanelimcontrolpanel.cpp
 * @brief LLPanelAvatar and related class implementations
 *
 * $LicenseInfo:firstyear=2004&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

// Original file: llpanelimcontrolpanel.cpp

#include "llviewerprecompiledheaders.h"

#include "llfloaterreg.h"

#include "fspanelimcontrolpanel.h"

#include "llagent.h"
#include "llappviewer.h" // for gDisconnected
#include "llavataractions.h"
#include "llavatariconctrl.h"
#include "llbutton.h"
#include "llgroupactions.h"
#include "llavatarlist.h"
#include "fsparticipantlist.h"
#include "llimview.h"
#include "llvoicechannel.h"
#include "llspeakers.h"

void FSPanelChatControlPanel::onCallButtonClicked()
{
	gIMMgr->startCall(mSessionId);
}

void FSPanelChatControlPanel::onEndCallButtonClicked()
{
	gIMMgr->endCall(mSessionId);
}

void FSPanelChatControlPanel::onOpenVoiceControlsClicked()
{
	LLFloaterReg::showInstance("fs_voice_controls");
}

void FSPanelChatControlPanel::onChange(EStatusType status, const std::string &channelURI, bool proximal)
{
	if(status == STATUS_JOINING || status == STATUS_LEFT_CHANNEL)
	{
		return;
	}

	updateCallButton();
}

void FSPanelChatControlPanel::onVoiceChannelStateChanged(const LLVoiceChannel::EState& old_state, const LLVoiceChannel::EState& new_state)
{
	updateButtons(new_state);
}

void FSPanelChatControlPanel::updateCallButton()
{
	// hide/show call button
	bool voice_enabled = LLVoiceClient::getInstance()->voiceEnabled() && LLVoiceClient::getInstance()->isVoiceWorking();

	LLIMModel::LLIMSession* session = LLIMModel::getInstance()->findIMSession(mSessionId);
	
	if (!session) 
	{
		getChildView("call_btn")->setEnabled(false);
		return;
	}

	bool session_initialized = session->mSessionInitialized;
	bool callback_enabled = session->mCallBackEnabled;

	BOOL enable_connect = session_initialized
		&& voice_enabled
		&& callback_enabled;
	getChildView("call_btn")->setEnabled(enable_connect);
}

void FSPanelChatControlPanel::updateButtons(LLVoiceChannel::EState state)
{
	bool is_call_started = state >= LLVoiceChannel::STATE_CALL_STARTED;
	getChildView("end_call_btn_panel")->setVisible( is_call_started);
	getChildView("voice_ctrls_btn_panel")->setVisible( is_call_started && findChild<LLView>("voice_ctrls_btn_panel"));
	getChildView("call_btn_panel")->setVisible( ! is_call_started);
	
	getChildView("volume_ctrl_panel")->setVisible(state == LLVoiceChannel::STATE_CONNECTED);
	
	updateCallButton();
}

FSPanelChatControlPanel::~FSPanelChatControlPanel()
{
}

BOOL FSPanelChatControlPanel::postBuild()
{
	return TRUE;
}

void FSPanelChatControlPanel::setSessionId(const LLUUID& session_id)
{
	//Method is called twice for AdHoc and Group chat. Second time when server init reply received
	mSessionId = session_id;
}

FSPanelIMControlPanel::FSPanelIMControlPanel()
{
}

FSPanelIMControlPanel::~FSPanelIMControlPanel()
{
}

BOOL FSPanelIMControlPanel::postBuild()
{
	childSetAction("mute_btn", boost::bind(&FSPanelIMControlPanel::onClickMuteVolume, this));
	childSetAction("block_btn", boost::bind(&FSPanelIMControlPanel::onClickBlock, this));
	childSetAction("unblock_btn", boost::bind(&FSPanelIMControlPanel::onClickUnblock, this));
	
	getChild<LLUICtrl>("volume_slider")->setCommitCallback(boost::bind(&FSPanelIMControlPanel::onVolumeChange, this, _2));

	setFocusReceivedCallback(boost::bind(&FSPanelIMControlPanel::onFocusReceived, this));
	
	return FSPanelChatControlPanel::postBuild();
}

void FSPanelIMControlPanel::draw()
{
	bool is_muted = LLMuteList::getInstance()->isMuted(mAvatarID);

	getChild<LLUICtrl>("block_btn_panel")->setVisible(!is_muted);
	getChild<LLUICtrl>("unblock_btn_panel")->setVisible(is_muted);

	if (getChildView("volume_ctrl_panel")->getVisible())
	{

		bool is_muted_voice = LLMuteList::getInstance()->isMuted(mAvatarID, LLMute::flagVoiceChat);

		LLUICtrl* mute_btn = getChild<LLUICtrl>("mute_btn");
		mute_btn->setValue( is_muted_voice );

		LLUICtrl* volume_slider = getChild<LLUICtrl>("volume_slider");
		volume_slider->setEnabled( !is_muted_voice );

		F32 volume;

		if (is_muted_voice)
		{
			// it's clearer to display their volume as zero
			volume = 0.f;
		}
		else
		{
			// actual volume
			volume = LLVoiceClient::getInstance()->getUserVolume(mAvatarID);
		}
		volume_slider->setValue( (F64)volume );
	}

	FSPanelChatControlPanel::draw();
}

void FSPanelIMControlPanel::onClickMuteVolume()
{
	// By convention, we only display and toggle voice mutes, not all mutes
	LLMuteList* mute_list = LLMuteList::getInstance();
	bool is_muted = mute_list->isMuted(mAvatarID, LLMute::flagVoiceChat);

	LLMute mute(mAvatarID, getChild<LLTextBox>("avatar_name")->getText(), LLMute::AGENT);
	if (!is_muted)
	{
		mute_list->add(mute, LLMute::flagVoiceChat);
	}
	else
	{
		mute_list->remove(mute, LLMute::flagVoiceChat);
	}
}

void FSPanelIMControlPanel::onClickBlock()
{
	LLMute mute(mAvatarID, getChild<LLTextBox>("avatar_name")->getText(), LLMute::AGENT);
	
	LLMuteList::getInstance()->add(mute);
}

void FSPanelIMControlPanel::onClickUnblock()
{
	LLMute mute(mAvatarID, getChild<LLTextBox>("avatar_name")->getText(), LLMute::AGENT);

	LLMuteList::getInstance()->remove(mute);
}

void FSPanelIMControlPanel::onVolumeChange(const LLSD& data)
{
	F32 volume = (F32)data.asReal();
	LLVoiceClient::getInstance()->setUserVolume(mAvatarID, volume);
}

void FSPanelIMControlPanel::onFocusReceived()
{
	// Disable all the buttons (Call, Teleport, etc) if disconnected.
	if (gDisconnected)
	{
		setAllChildrenEnabled(FALSE);
	}
}

void FSPanelIMControlPanel::setSessionId(const LLUUID& session_id)
{
	FSPanelChatControlPanel::setSessionId(session_id);

	LLIMModel& im_model = LLIMModel::instance();

	mAvatarID = im_model.getOtherParticipantID(session_id);
}

//virtual
void FSPanelIMControlPanel::changed(U32 mask)
{
	getChildView("add_friend_btn")->setEnabled(!LLAvatarActions::isFriend(mAvatarID));
	
	// Disable "Teleport" button if friend is offline
	if(LLAvatarActions::isFriend(mAvatarID))
	{
		getChildView("teleport_btn")->setEnabled(LLAvatarTracker::instance().isBuddyOnline(mAvatarID));
	}
}

void FSPanelIMControlPanel::onNameCache(const LLUUID& id, const std::string& full_name, bool is_group)
{
	if ( id == mAvatarID )
	{
		std::string avatar_name = full_name;
		getChild<LLTextBox>("avatar_name")->setValue(avatar_name);
		getChild<LLTextBox>("avatar_name")->setToolTip(avatar_name);

		bool is_linden = LLStringUtil::endsWith(full_name, " Linden");
		getChild<LLUICtrl>("mute_btn")->setEnabled( !is_linden);
	}
}

FSPanelGroupControlPanel::FSPanelGroupControlPanel(const LLUUID& session_id):
mParticipantList(NULL)
{
}

BOOL FSPanelGroupControlPanel::postBuild()
{
	return FSPanelChatControlPanel::postBuild();
}

FSPanelGroupControlPanel::~FSPanelGroupControlPanel()
{
	delete mParticipantList;
	mParticipantList = NULL;
}

// virtual
void FSPanelGroupControlPanel::draw()
{
	// Need to resort the participant list if it's in sort by recent speaker order.
	if (mParticipantList)
		mParticipantList->update();
	FSPanelChatControlPanel::draw();
}

void FSPanelGroupControlPanel::onGroupInfoButtonClicked()
{
	LLGroupActions::show(mGroupID);
}

void FSPanelGroupControlPanel::onSortMenuItemClicked(const LLSD& userdata)
{
	// TODO: Check this code when when sort order menu will be added. (EM)
	if (false && !mParticipantList)
		return;

	std::string chosen_item = userdata.asString();

	if (chosen_item == "sort_name")
	{
		mParticipantList->setSortOrder(LLParticipantList::E_SORT_BY_NAME);
	}

}

void FSPanelGroupControlPanel::onVoiceChannelStateChanged(const LLVoiceChannel::EState& old_state, const LLVoiceChannel::EState& new_state)
{
	FSPanelChatControlPanel::onVoiceChannelStateChanged(old_state, new_state);
	mParticipantList->setSpeakingIndicatorsVisible(new_state >= LLVoiceChannel::STATE_CALL_STARTED);
}

void FSPanelGroupControlPanel::setSessionId(const LLUUID& session_id)
{
	FSPanelChatControlPanel::setSessionId(session_id);

	mGroupID = session_id;

	// for group and Ad-hoc chat we need to include agent into list 
	if(!mParticipantList)
	{
		LLSpeakerMgr* speaker_manager = LLIMModel::getInstance()->getSpeakerManager(session_id);
		mParticipantList = new LLParticipantList(speaker_manager, getChild<LLAvatarList>("grp_speakers_list"), true,false);
	}
}


FSPanelAdHocControlPanel::FSPanelAdHocControlPanel(const LLUUID& session_id):FSPanelGroupControlPanel(session_id)
{
}

BOOL FSPanelAdHocControlPanel::postBuild()
{
	//We don't need LLPanelGroupControlPanel::postBuild() to be executed as there is no group_info_btn at AdHoc chat
	return FSPanelChatControlPanel::postBuild();
}
