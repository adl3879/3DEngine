using Coral.Managed.Interop;
using System.Numerics;

namespace ScriptCore.Net
{
	public class IComponent
	{
		public int EntityID { get; protected set; }
	}

	public class TransformComponent : IComponent
	{
		public TransformComponent(int entityID)
		{
			EntityID = entityID;
		}

		internal static unsafe delegate*<int, float, float, float, void> SetTranslationICall;
		internal static unsafe delegate*<int, NativeArray<float>> GetTranslationICall;
		internal static unsafe delegate*<int, float, float, float, void> SetRotationICall;
		internal static unsafe delegate*<int, NativeArray<float>> GetRotationICall;
		internal static unsafe delegate*<int, float, float, float, void> SetScaleICall;
		internal static unsafe delegate*<int, NativeArray<float>> GetScaleICall;

		public Vector3 Translation
		{
			get
			{
				NativeArray<float> result;
				unsafe { result = GetTranslationICall(EntityID); }
				return new Vector3(result[0], result[1], result[2]);
			}
			set
			{
				unsafe { SetTranslationICall(EntityID, value.X, value.Y, value.Z); }
			}
		}

		public Vector3 Rotation
		{
			get
			{
				NativeArray<float> result;
				unsafe { result = GetRotationICall(EntityID); }
				return new Vector3(result[0], result[1], result[2]);
			}
			set
			{
				unsafe { SetRotationICall(EntityID, value.X, value.Y, value.Z); }
			}
		}

		public Vector3 Scale
		{
			get
			{
				NativeArray<float> result;
				unsafe { result = GetScaleICall(EntityID); }
				return new Vector3(result[0], result[1], result[2]);
			}
			set
			{
				unsafe { SetScaleICall(EntityID, value.X, value.Y, value.Z); }
			}
		}

		internal static unsafe delegate*<int, float, float, float, void> TranslateICall;
		internal static unsafe delegate*<int, float, float, float, void> RotateICall;
		internal static unsafe delegate*<int, float, float, float, void> ScaleICall;

		public void Translate(Vector3 translation)
		{
			unsafe { TranslateICall(EntityID, translation.X, translation.Y, translation.Z); }
		}

		public void Rotate(Vector3 rotation)
		{
			unsafe { RotateICall(EntityID, rotation.X, rotation.Y, rotation.Z); }
		}

		public void ScaleBy(Vector3 scale)
		{
			unsafe { ScaleICall(EntityID, scale.X, scale.Y, scale.Z); }
		}
	}
}
