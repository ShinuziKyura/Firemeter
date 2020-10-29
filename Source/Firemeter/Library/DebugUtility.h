// Copyright 2020-2021 Ricardo Santos. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/StringBuilder.h"

// TODO find out how to clear log history from time to time

//////////////////////////////////////////////////
/// Log utilities

struct NonMemberFunction_Tag;

struct FLogHelper
{
	template <size_t SizeValue>
	struct FVariadicMacroHelper
	{
		FVariadicMacroHelper(TCHAR const(& InFormat)[SizeValue])
			: Format{ InFormat }
		{
		}

		template <class ... ArgTypes>
		FString Printf(ArgTypes && ... Args)
		{
			return FString::Printf(Format, Forward<ArgTypes>(Args) ...);
		}

		TCHAR const(& Format)[SizeValue];
	};

	template <size_t SizeValue>
	static FVariadicMacroHelper<SizeValue> FormatLogArgs(TCHAR const(& InFormat)[SizeValue])
	{
		return FVariadicMacroHelper<SizeValue>(InFormat);
	}
	
	static FString FormatLogInfo(FString && InDateTime, bool bShowDateTime,
		FString && InFunction, bool bShowFunction,
		FString && InFilePath, int32 InFileLine, bool bShowFileInfo)
	{
		// Format: [ $datetime // $function | file: $filepath // line: $fileline ]

		if (!bShowDateTime && !bShowFunction && !bShowFileInfo)
		{
			return TEXT("////////");
		}

		TStringBuilder<256> Buffer;

		Buffer.Append(TEXT("[ "));

		if (bShowDateTime)
		{
			Buffer.Append(InDateTime);

			if (bShowFunction)
			{
				Buffer.Append(TEXT(" // "));
			}
			else if (bShowFileInfo)
			{
				Buffer.Append(TEXT(" | "));
			}
		}

		if (bShowFunction)
		{
			Buffer.Append(InFunction);

			if (bShowFileInfo)
			{
				Buffer.Append(TEXT(" | "));
			}
		}

		if (bShowFileInfo)
		{
			Buffer.Appendf(TEXT("file: %s // line: %d"), *InFilePath, InFileLine);
		}

		Buffer.Append(TEXT(" ]"));

		return Buffer.ToString();
	}

	template <class ClassType>
	static FString GetQualifiedFunctionName(FString && InClassName, FString && InFunctionName)
	{
		static_assert(TIsVoidType<ClassType>::Value || TIsClass<ClassType>::Value);

		return TIsSame<ClassType, NonMemberFunction_Tag>::Value
			? InFunctionName
			: InClassName.Append(TEXT("::")).Append(InFunctionName);
	}

	static FString GetPrefixedClassName(UClass * InClass)
	{
		if (InClass->IsChildOf(AActor::StaticClass()))
		{
			return FString::Printf(TEXT("A%s"), *InClass->GetName());
		}

		if (InClass->IsChildOf(UObject::StaticClass()))
		{
			return FString::Printf(TEXT("U%s"), *InClass->GetName());
		}

		return FString::Printf(TEXT("F%s"), *InClass->GetName());
	}

	static FString GetSourceRelativePath(FString && InPath)
	{
		auto const Path = InPath.Replace(TEXT("\\"), TEXT("/"));
		auto const SourceIndex = Path.Find(TEXT("/Source"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);

		if (SourceIndex != INDEX_NONE)
		{
			return Path.Mid(SourceIndex + 8);
		}

		return Path;
	}
};

#define FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL_FM_SUPPRESS_LOG_DATETIME() true
#define FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL_() false
#define FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL_0() true
#define FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL_1() false
#define FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL_false() true
#define FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL_true() false
#define FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL(...) FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL_ ## __VA_ARGS__()
#define FM_EVAL_SUPPRESS_LOG_DATETIME(SuppressMacro) FM_EVAL_SUPPRESS_LOG_DATETIME_INTERNAL(SuppressMacro)

#define FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL_FM_SUPPRESS_LOG_FUNCTION() true
#define FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL_() false
#define FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL_0() true
#define FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL_1() false
#define FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL_false() true
#define FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL_true() false
#define FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL(...) FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL_ ## __VA_ARGS__()
#define FM_EVAL_SUPPRESS_LOG_FUNCTION(SuppressMacro) FM_EVAL_SUPPRESS_LOG_FUNCTION_INTERNAL(SuppressMacro)

#define FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL_FM_SUPPRESS_LOG_FILEINFO() true
#define FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL_() false
#define FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL_0() true
#define FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL_1() false
#define FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL_false() true
#define FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL_true() false
#define FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL(...) FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL_ ## __VA_ARGS__()
#define FM_EVAL_SUPPRESS_LOG_FILEINFO(SuppressMacro) FM_EVAL_SUPPRESS_LOG_FILEINFO_INTERNAL(SuppressMacro)

#define FM_EVAL_LOG_INTERNAL(Category, Verbosity, ClassType, ClassTypeName, Format, ...) \
	UE_LOG(Category, Verbosity, TEXT("%s\n>\t%s"),\
		*FLogHelper::FormatLogInfo(\
			FDateTime::Now().ToString(TEXT("%y-%m-%d@%Hh%Mm%S.%ss")),\
			FM_EVAL_SUPPRESS_LOG_DATETIME(FM_SUPPRESS_LOG_DATETIME),\
			FLogHelper::GetQualifiedFunctionName<ClassType>(ClassTypeName, ANSI_TO_TCHAR(__func__)),\
			FM_EVAL_SUPPRESS_LOG_FUNCTION(FM_SUPPRESS_LOG_FUNCTION),\
			FLogHelper::GetSourceRelativePath(TEXT(__FILE__)),\
			__LINE__,\
			FM_EVAL_SUPPRESS_LOG_FILEINFO(FM_SUPPRESS_LOG_FILEINFO)\
		),\
		*FLogHelper::FormatLogArgs(Format)\
			.Printf(__VA_ARGS__)\
			.Replace(TEXT("\n"), TEXT("\n>\t"))\
	)

#define FM_ULOG_INTERNAL(Category, Verbosity, Format, ...) \
	FM_EVAL_LOG_INTERNAL(Category, Verbosity, void, FLogHelper::GetPrefixedClassName(GetParentNativeClass(GetClass())), Format, __VA_ARGS__)

#define FM_LOG_INTERNAL(Category, Verbosity, Class, Format, ...) \
	FM_EVAL_LOG_INTERNAL(Category, Verbosity, Class, TEXT(#Class), Format, __VA_ARGS__)

#define FM_ULOG(Category, Format, ...) FM_ULOG_INTERNAL(Category, Log, Format, __VA_ARGS__)
#define FM_UWARNING(Category, Format, ...) FM_ULOG_INTERNAL(Category, Warning, Format, __VA_ARGS__)
#define FM_UERROR(Category, Format, ...) FM_ULOG_INTERNAL(Category, Error, Format, __VA_ARGS__)

#define FM_LOG(Category, Class, Format, ...) FM_LOG_INTERNAL(Category, Log, Class, Format, __VA_ARGS__)
#define FM_WARNING(Category, Class, Format, ...) FM_LOG_INTERNAL(Category, Warning, Class, Format, __VA_ARGS__)
#define FM_ERROR(Category, Class, Format, ...) FM_LOG_INTERNAL(Category, Error, Class, Format, __VA_ARGS__)

//////////////////////////////////////////////////
/// Stat utilities

class FStatGroupRegistry
{
public:
	static void RegisterStatGroup(FString && InStatId)
	{
		StatGroups.Add(InStatId);
	}

	static TArray<FString> const & QueryStatGroups()
	{
		return StatGroups;
	}

private:
	static inline TArray<FString> StatGroups;

};

#define FM_DECLARE_STATS_GROUP(StatGroupId, Description) \
	DECLARE_STATS_GROUP(Description, STATGROUP_ ## StatGroupId, STATCAT_Advanced);\
	namespace Firemeter_Internal\
	{\
		static struct FStatGroupEntry_ ## StatGroupId\
		{\
			FStatGroupEntry_ ## StatGroupId()\
			{\
				FStatGroupRegistry::RegisterStatGroup(ANSI_TO_TCHAR(#StatGroupId));\
			}\
		}\
		StatGroupEntry_ ## StatGroupId;\
	}

#define FM_DECLARE_CYCLE_STAT(StatGroupId, StatId, Description) \
	DECLARE_CYCLE_STAT(Description, StatId, STATGROUP_ ## StatGroupId)

//////////////////////////////////////////////////
/// Development utilities

#define FM_LIVE_CODING_TICK_GUARD(InvariantTest, RecoveryTest) \
{\
	if (WITH_EDITOR && !ensure(InvariantTest()))\
	{\
		RecoveryTest();\
		if (!InvariantTest())\
		{\
			return;\
		}\
	}\
}
