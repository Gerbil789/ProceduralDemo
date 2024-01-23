// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ProceduralDemo/NoiseGeneratorComponent.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeNoiseGeneratorComponent() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_UActorComponent();
	PROCEDURALDEMO_API UClass* Z_Construct_UClass_UNoiseGeneratorComponent();
	PROCEDURALDEMO_API UClass* Z_Construct_UClass_UNoiseGeneratorComponent_NoRegister();
	UPackage* Z_Construct_UPackage__Script_ProceduralDemo();
// End Cross Module References
	void UNoiseGeneratorComponent::StaticRegisterNativesUNoiseGeneratorComponent()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UNoiseGeneratorComponent);
	UClass* Z_Construct_UClass_UNoiseGeneratorComponent_NoRegister()
	{
		return UNoiseGeneratorComponent::StaticClass();
	}
	struct Z_Construct_UClass_UNoiseGeneratorComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UNoiseGeneratorComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UActorComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_ProceduralDemo,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UNoiseGeneratorComponent_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UNoiseGeneratorComponent_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Custom" },
		{ "IncludePath", "NoiseGeneratorComponent.h" },
		{ "ModuleRelativePath", "NoiseGeneratorComponent.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UNoiseGeneratorComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UNoiseGeneratorComponent>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UNoiseGeneratorComponent_Statics::ClassParams = {
		&UNoiseGeneratorComponent::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x00B000A4u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UNoiseGeneratorComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_UNoiseGeneratorComponent_Statics::Class_MetaDataParams)
	};
	UClass* Z_Construct_UClass_UNoiseGeneratorComponent()
	{
		if (!Z_Registration_Info_UClass_UNoiseGeneratorComponent.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UNoiseGeneratorComponent.OuterSingleton, Z_Construct_UClass_UNoiseGeneratorComponent_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UNoiseGeneratorComponent.OuterSingleton;
	}
	template<> PROCEDURALDEMO_API UClass* StaticClass<UNoiseGeneratorComponent>()
	{
		return UNoiseGeneratorComponent::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UNoiseGeneratorComponent);
	UNoiseGeneratorComponent::~UNoiseGeneratorComponent() {}
	struct Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_NoiseGeneratorComponent_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_NoiseGeneratorComponent_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UNoiseGeneratorComponent, UNoiseGeneratorComponent::StaticClass, TEXT("UNoiseGeneratorComponent"), &Z_Registration_Info_UClass_UNoiseGeneratorComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UNoiseGeneratorComponent), 440018107U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_NoiseGeneratorComponent_h_3914688120(TEXT("/Script/ProceduralDemo"),
		Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_NoiseGeneratorComponent_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_NoiseGeneratorComponent_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
