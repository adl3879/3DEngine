namespace ScriptCore.Net
{
	public class Entity
	{
		internal static unsafe delegate*<int, int, bool> EntityHasComponentICall;

		public enum ComponentTypes
		{
			UNKNOWN = -1,
			TRANSFORM = 0,
			MESH,
			NET_SCRIPT,
			POINT_LIGHT,
			SPOT_LIGHT,
			DIRECTIONAL_LIGHT,
			CAMERA,
			BOX_COLLIDER,
			SPHERE_COLLIDER,
			CAPSULE_COLLIDER,
			RIGIDBODY,
		}

		public int ID { get; set; }
		public int ECSHandle { get; set; }

		public virtual void OnInit() { }
		public virtual void OnUpdate(float dt) { }
		public virtual void OnFixedUpdate(float dt) { }
		public virtual void OnDestroy() { }

		protected static Dictionary<Type, ComponentTypes> MappingTypeEnum = new Dictionary<Type, ComponentTypes>()
		{
			{ typeof(TransformComponent), ComponentTypes.TRANSFORM },
		};

		public bool HasComponent<T>()
		{
			if (MappingTypeEnum.ContainsKey(typeof(T)))
			{
				unsafe { return EntityHasComponentICall(ECSHandle, (int)MappingTypeEnum[typeof(T)]); };
			}

			return false;
		}

		public T? GetComponent<T>() where T : IComponent
		{
			if (HasComponent<T>())
			{
				return (T?)Activator.CreateInstance(typeof(T), ECSHandle);
			}

			return default(T);
		}
	}
}
