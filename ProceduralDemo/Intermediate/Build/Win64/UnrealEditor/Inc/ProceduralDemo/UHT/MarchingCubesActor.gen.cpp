// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ProceduralDemo/MarchingCubesActor.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeMarchingCubesActor() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
	PROCEDURALDEMO_API UClass* Z_Construct_UClass_AMarchingCubesActor();
	PROCEDURALDEMO_API UClass* Z_Construct_UClass_AMarchingCubesActor_NoRegister();
	UPackage* Z_Construct_UPackage__Script_ProceduralDemo();
// End Cross Module References
	DEFINE_FUNCTION(AMarchingCubesActor::execCleanUpMesh)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->CleanUpMesh();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(AMarchingCubesActor::execGenerateMesh)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->GenerateMesh();
		P_NATIVE_END;
	}
	void AMarchingCubesActor::StaticRegisterNativesAMarchingCubesActor()
	{
		UClass* Class = AMarchingCubesActor::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "CleanUpMesh", &AMarchingCubesActor::execCleanUpMesh },
			{ "GenerateMesh", &AMarchingCubesActor::execGenerateMesh },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_AMarchingCubesActor_CleanUpMesh_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AMarchingCubesActor_CleanUpMesh_Statics::Function_MetaDataParams[] = {
		{ "Category", "MarchingCubes" },
		{ "ModuleRelativePath", "MarchingCubesActor.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AMarchingCubesActor_CleanUpMesh_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AMarchingCubesActor, nullptr, "CleanUpMesh", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AMarchingCubesActor_CleanUpMesh_Statics::Function_MetaDataParams), Z_Construct_UFunction_AMarchingCubesActor_CleanUpMesh_Statics::Function_MetaDataParams) };
	UFunction* Z_Construct_UFunction_AMarchingCubesActor_CleanUpMesh()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AMarchingCubesActor_CleanUpMesh_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AMarchingCubesActor_GenerateMesh_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AMarchingCubesActor_GenerateMesh_Statics::Function_MetaDataParams[] = {
		{ "Category", "MarchingCubes" },
		{ "ModuleRelativePath", "MarchingCubesActor.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AMarchingCubesActor_GenerateMesh_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AMarchingCubesActor, nullptr, "GenerateMesh", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AMarchingCubesActor_GenerateMesh_Statics::Function_MetaDataParams), Z_Construct_UFunction_AMarchingCubesActor_GenerateMesh_Statics::Function_MetaDataParams) };
	UFunction* Z_Construct_UFunction_AMarchingCubesActor_GenerateMesh()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AMarchingCubesActor_GenerateMesh_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AMarchingCubesActor);
	UClass* Z_Construct_UClass_AMarchingCubesActor_NoRegister()
	{
		return AMarchingCubesActor::StaticClass();
	}
	struct Z_Construct_UClass_AMarchingCubesActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CustomMesh_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_CustomMesh;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AMarchingCubesActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_ProceduralDemo,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AMarchingCubesActor_Statics::DependentSingletons) < 16);
	const FClassFunctionLinkInfo Z_Construct_UClass_AMarchingCubesActor_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_AMarchingCubesActor_CleanUpMesh, "CleanUpMesh" }, // 3836338938
		{ &Z_Construct_UFunction_AMarchingCubesActor_GenerateMesh, "GenerateMesh" }, // 2067150008
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AMarchingCubesActor_Statics::FuncInfo) < 2048);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AMarchingCubesActor_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "MarchingCubesActor.h" },
		{ "ModuleRelativePath", "MarchingCubesActor.h" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AMarchingCubesActor_Statics::NewProp_CustomMesh_MetaData[] = {
		{ "Category", "Mesh" },
		{ "ModuleRelativePath", "MarchingCubesActor.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AMarchingCubesActor_Statics::NewProp_CustomMesh = { "CustomMesh", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AMarchingCubesActor, CustomMesh), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AMarchingCubesActor_Statics::NewProp_CustomMesh_MetaData), Z_Construct_UClass_AMarchingCubesActor_Statics::NewProp_CustomMesh_MetaData) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AMarchingCubesActor_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AMarchingCubesActor_Statics::NewProp_CustomMesh,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AMarchingCubesActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AMarchingCubesActor>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_AMarchingCubesActor_Statics::ClassParams = {
		&AMarchingCubesActor::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_AMarchingCubesActor_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_AMarchingCubesActor_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AMarchingCubesActor_Statics::Class_MetaDataParams), Z_Construct_UClass_AMarchingCubesActor_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AMarchingCubesActor_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_AMarchingCubesActor()
	{
		if (!Z_Registration_Info_UClass_AMarchingCubesActor.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AMarchingCubesActor.OuterSingleton, Z_Construct_UClass_AMarchingCubesActor_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_AMarchingCubesActor.OuterSingleton;
	}
	template<> PROCEDURALDEMO_API UClass* StaticClass<AMarchingCubesActor>()
	{
		return AMarchingCubesActor::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(AMarchingCubesActor);
	AMarchingCubesActor::~AMarchingCubesActor() {}
	struct Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_MarchingCubesActor_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_MarchingCubesActor_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_AMarchingCubesActor, AMarchingCubesActor::StaticClass, TEXT("AMarchingCubesActor"), &Z_Registration_Info_UClass_AMarchingCubesActor, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AMarchingCubesActor), 828140573U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_MarchingCubesActor_h_3602372889(TEXT("/Script/ProceduralDemo"),
		Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_MarchingCubesActor_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_vojta_source_repos_ProceduralDemo_ProceduralDemo_Source_ProceduralDemo_MarchingCubesActor_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
