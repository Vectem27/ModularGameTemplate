// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TP_MGFEditorTarget : TargetRules
{
	public TP_MGFEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

        ExtraModuleNames.Add("ModularGameFramework" /* ,"ModularGameFrameworkEditor" */);

        TP_MGFTarget.ApplySharedTP_MGFTargetSettings(this);

        // This is used for touch screen development along with the "Unreal Remote 2" app
        EnablePlugins.Add("RemoteSession");

        ProjectDefinitions.Add("MODIO_DEVELOPMENT_MODE");
    }
}
