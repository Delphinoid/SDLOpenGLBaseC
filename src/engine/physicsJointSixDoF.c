/**void SixDOFConstraint::UpdateTranslationLimits(){
	if(mLimitMin[0] > mLimitMax[0]){
		mLimitMin[0] = 0.f;
		mLimitMax[0] = 0.f;
	}
	if(mLimitMin[1] > mLimitMax[1]){
		mLimitMin[1] = 0.f;
		mLimitMax[1] = 0.f;
	}
	if(mLimitMin[2] > mLimitMax[2]){
		mLimitMin[2] = 0.f;
		mLimitMax[2] = 0.f;
	}
}

void SixDOFConstraint::UpdateRotationLimits(){

	if (mSwingTwistConstraintPart.GetSwingType() == ESwingType::Cone){
		// Cone swing upper limit needs to be positive
		mLimitMax[EAxis::RotationY] = floatMaxFast(0.f, mLimitMax[EAxis::RotationY]);
		mLimitMax[EAxis::RotationZ] = floatMaxFast(0.f, mLimitMax[EAxis::RotationZ]);
		// Cone swing limits only support symmetric ranges
		mLimitMin[EAxis::RotationY] = -mLimitMax[EAxis::RotationY];
		mLimitMin[EAxis::RotationZ] = -mLimitMax[EAxis::RotationZ];
	}

	for (int i = EAxis::RotationX; i <= EAxis::RotationZ; ++i){
		// Clamp to [-PI, PI] range
		mLimitMin[i] = Clamp(mLimitMin[i], -M_PI, M_PI);
		mLimitMax[i] = Clamp(mLimitMax[i], -M_PI, M_PI);
		// Set to zero if the limits are inversed
		if(mLimitMin[i] > mLimitMax[i]){
			mLimitMin[i] = mLimitMax[i] = 0.f;
        }
	}

	// Pass limits on to constraint part
	mSwingTwistConstraintPart.SetLimits(mLimitMin[EAxis::RotationX], mLimitMax[EAxis::RotationX], mLimitMin[EAxis::RotationY], mLimitMax[EAxis::RotationY], mLimitMin[EAxis::RotationZ], mLimitMax[EAxis::RotationZ]);

}

void SixDOFConstraint::UpdateFixedFreeAxis(){

	flags_t old_free_axis = mFreeAxis;
	flags_t old_fixed_axis = mFixedAxis;

	// Cache which axis are fixed and which ones are free
	mFreeAxis = 0;
	mFixedAxis = 0;
	for (int a = 0; a < EAxis::Num; ++a)
	{
		float limit = a >= EAxis::RotationX? JPH_PI : FLT_MAX;

		if (mLimitMin[a] >= mLimitMax[a])
			mFixedAxis |= 1 << a;
		else if (mLimitMin[a] <= -limit && mLimitMax[a] >= limit)
			mFreeAxis |= 1 << a;
	}

	// On change we deactivate all constraints to reset warm starting
	if (old_free_axis != mFreeAxis || old_fixed_axis != mFixedAxis)
	{
		for (AxisConstraintPart &c : mTranslationConstraintPart)
			c.Deactivate();
		mPointConstraintPart.Deactivate();
		mSwingTwistConstraintPart.Deactivate();
		mRotationConstraintPart.Deactivate();
		for (AxisConstraintPart &c : mMotorTranslationConstraintPart)
			c.Deactivate();
		for (AngleConstraintPart &c : mMotorRotationConstraintPart)
			c.Deactivate();
	}
}

SixDOFConstraint::SixDOFConstraint(Body &inBody1, Body &inBody2, const SixDOFConstraintSettings &inSettings) :
	TwoBodyConstraint(inBody1, inBody2, inSettings)
{
	// Override swing type
	mSwingTwistConstraintPart.SetSwingType(inSettings.mSwingType);

	// Calculate rotation needed to go from constraint space to body1 local space
	Vec3 axis_z1 = inSettings.mAxisX1.Cross(inSettings.mAxisY1);
	Mat44 c_to_b1(Vec4(inSettings.mAxisX1, 0), Vec4(inSettings.mAxisY1, 0), Vec4(axis_z1, 0), Vec4(0, 0, 0, 1));
	mConstraintToBody1 = c_to_b1.GetQuaternion();

	// Calculate rotation needed to go from constraint space to body2 local space
	Vec3 axis_z2 = inSettings.mAxisX2.Cross(inSettings.mAxisY2);
	Mat44 c_to_b2(Vec4(inSettings.mAxisX2, 0), Vec4(inSettings.mAxisY2, 0), Vec4(axis_z2, 0), Vec4(0, 0, 0, 1));
	mConstraintToBody2 = c_to_b2.GetQuaternion();

	if (inSettings.mSpace == EConstraintSpace::WorldSpace)
	{
		// If all properties were specified in world space, take them to local space now
		mLocalSpacePosition1 = Vec3(inBody1.GetInverseCenterOfMassTransform() * inSettings.mPosition1);
		mConstraintToBody1 = inBody1.GetRotation().Conjugated() * mConstraintToBody1;

		mLocalSpacePosition2 = Vec3(inBody2.GetInverseCenterOfMassTransform() * inSettings.mPosition2);
		mConstraintToBody2 = inBody2.GetRotation().Conjugated() * mConstraintToBody2;
	}
	else
	{
		mLocalSpacePosition1 = Vec3(inSettings.mPosition1);
		mLocalSpacePosition2 = Vec3(inSettings.mPosition2);
	}

	// Copy translation and rotation limits
	memcpy(mLimitMin, inSettings.mLimitMin, sizeof(mLimitMin));
	memcpy(mLimitMax, inSettings.mLimitMax, sizeof(mLimitMax));
	memcpy(mLimitsSpringSettings, inSettings.mLimitsSpringSettings, sizeof(mLimitsSpringSettings));
	UpdateTranslationLimits();
	UpdateRotationLimits();
	UpdateFixedFreeAxis();
	CacheHasSpringLimits();

	// Store friction settings
	memcpy(mMaxFriction, inSettings.mMaxFriction, sizeof(mMaxFriction));

	// Store motor settings
	for (int i = 0; i < EAxis::Num; ++i)
		mMotorSettings[i] = inSettings.mMotorSettings[i];

	// Cache if motors are active (motors are off initially, but we may have friction)
	CacheTranslationMotorActive();
	CacheRotationMotorActive();
}

void SixDOFConstraint::NotifyShapeChanged(const BodyID &inBodyID, Vec3Arg inDeltaCOM)
{
	if (mBody1->GetID() == inBodyID)
		mLocalSpacePosition1 -= inDeltaCOM;
	else if (mBody2->GetID() == inBodyID)
		mLocalSpacePosition2 -= inDeltaCOM;
}

void SixDOFConstraint::SetTranslationLimits(Vec3Arg inLimitMin, Vec3Arg inLimitMax)
{
	mLimitMin[EAxis::TranslationX] = inLimitMin.GetX();
	mLimitMin[EAxis::TranslationY] = inLimitMin.GetY();
	mLimitMin[EAxis::TranslationZ] = inLimitMin.GetZ();
	mLimitMax[EAxis::TranslationX] = inLimitMax.GetX();
	mLimitMax[EAxis::TranslationY] = inLimitMax.GetY();
	mLimitMax[EAxis::TranslationZ] = inLimitMax.GetZ();

	UpdateTranslationLimits();
	UpdateFixedFreeAxis();
}

void SixDOFConstraint::SetRotationLimits(Vec3Arg inLimitMin, Vec3Arg inLimitMax)
{
	mLimitMin[EAxis::RotationX] = inLimitMin.GetX();
	mLimitMin[EAxis::RotationY] = inLimitMin.GetY();
	mLimitMin[EAxis::RotationZ] = inLimitMin.GetZ();
	mLimitMax[EAxis::RotationX] = inLimitMax.GetX();
	mLimitMax[EAxis::RotationY] = inLimitMax.GetY();
	mLimitMax[EAxis::RotationZ] = inLimitMax.GetZ();

	UpdateRotationLimits();
	UpdateFixedFreeAxis();
}

void SixDOFConstraint::SetMaxFriction(EAxis inAxis, float inFriction)
{
	mMaxFriction[inAxis] = inFriction;

	if (inAxis >= EAxis::TranslationX && inAxis <= EAxis::TranslationZ)
		CacheTranslationMotorActive();
	else
		CacheRotationMotorActive();
}

void SixDOFConstraint::GetPositionConstraintProperties(Vec3 &outR1PlusU, Vec3 &outR2, Vec3 &outU) const
{
	RVec3 p1 = mBody1->GetCenterOfMassTransform() * mLocalSpacePosition1;
	RVec3 p2 = mBody2->GetCenterOfMassTransform() * mLocalSpacePosition2;
	outR1PlusU = Vec3(p2 - mBody1->GetCenterOfMassPosition()); // r1 + u = (p1 - x1) + (p2 - p1) = p2 - x1
	outR2 = Vec3(p2 - mBody2->GetCenterOfMassPosition());
	outU = Vec3(p2 - p1);
}

Quat SixDOFConstraint::GetRotationInConstraintSpace() const
{
	// Let b1, b2 be the center of mass transform of body1 and body2 (For body1 this is mBody1->GetCenterOfMassTransform())
	// Let c1, c2 be the transform that takes a vector from constraint space to local space of body1 and body2 (For body1 this is Mat44::sRotationTranslation(mConstraintToBody1, mLocalSpacePosition1))
	// Let q be the rotation of the constraint in constraint space
	// b2 takes a vector from the local space of body2 to world space
	// To express this in terms of b1: b2 = b1 * c1 * q * c2^-1
	// c2^-1 goes from local body 2 space to constraint space
	// q rotates the constraint
	// c1 goes from constraint space to body 1 local space
	// b1 goes from body 1 local space to world space
	// So when the body rotations are given, q = (b1 * c1)^-1 * b2 c2
	// Or: q = (q1 * c1)^-1 * (q2 * c2) if we're only interested in rotations
	return (mBody1->GetRotation() * mConstraintToBody1).Conjugated() * mBody2->GetRotation() * mConstraintToBody2;
}

void SixDOFConstraint::CacheTranslationMotorActive()
{
	mTranslationMotorActive = mMotorState[EAxis::TranslationX] != EMotorState::Off
		|| mMotorState[EAxis::TranslationY] != EMotorState::Off
		|| mMotorState[EAxis::TranslationZ] != EMotorState::Off
		|| HasFriction(EAxis::TranslationX)
		|| HasFriction(EAxis::TranslationY)
		|| HasFriction(EAxis::TranslationZ);
}

void SixDOFConstraint::CacheRotationMotorActive()
{
	mRotationMotorActive = mMotorState[EAxis::RotationX] != EMotorState::Off
		|| mMotorState[EAxis::RotationY] != EMotorState::Off
		|| mMotorState[EAxis::RotationZ] != EMotorState::Off
		|| HasFriction(EAxis::RotationX)
		|| HasFriction(EAxis::RotationY)
		|| HasFriction(EAxis::RotationZ);
}

void SixDOFConstraint::CacheRotationPositionMotorActive()
{
	mRotationPositionMotorActive = 0;
	for (int i = 0; i < 3; ++i)
		if (mMotorState[EAxis::RotationX + i] == EMotorState::Position)
			mRotationPositionMotorActive |= 1 << i;
}

void SixDOFConstraint::CacheHasSpringLimits()
{
	mHasSpringLimits = mLimitsSpringSettings[EAxis::TranslationX].mFrequency > 0.0f
		|| mLimitsSpringSettings[EAxis::TranslationY].mFrequency > 0.0f
		|| mLimitsSpringSettings[EAxis::TranslationZ].mFrequency > 0.0f;
}

void SixDOFConstraint::SetMotorState(EAxis inAxis, EMotorState inState)
{
	JPH_ASSERT(inState == EMotorState::Off || mMotorSettings[inAxis].IsValid());

	if (mMotorState[inAxis] != inState)
	{
		mMotorState[inAxis] = inState;

		// Ensure that warm starting next frame doesn't apply any impulses (motor parts are repurposed for different modes)
		if (inAxis >= EAxis::TranslationX && inAxis <= EAxis::TranslationZ)
		{
			mMotorTranslationConstraintPart[inAxis - EAxis::TranslationX].Deactivate();

			CacheTranslationMotorActive();
		}
		else
		{
			JPH_ASSERT(inAxis >= EAxis::RotationX && inAxis <= EAxis::RotationZ);

			mMotorRotationConstraintPart[inAxis - EAxis::RotationX].Deactivate();

			CacheRotationMotorActive();
			CacheRotationPositionMotorActive();
		}
	}
}

void SixDOFConstraint::SetTargetOrientationCS(QuatArg inOrientation)
{
	Quat q_swing, q_twist;
	inOrientation.GetSwingTwist(q_swing, q_twist);

	uint clamped_axis;
	mSwingTwistConstraintPart.ClampSwingTwist(q_swing, q_twist, clamped_axis);

	if (clamped_axis != 0)
		mTargetOrientation = q_swing * q_twist;
	else
		mTargetOrientation = inOrientation;
}

void physJointSixDoFPresolveConstraints(physJoint *const __RESTRICT__ joint, physRigidBody *const __RESTRICT__ bodyA, physRigidBody *const __RESTRICT__ bodyB, const float dt_s){

	// Get body rotations
	const quat orientationA = bodyA->configuration.orientation;
	const quat orientationB = bodyB->configuration.orientation;

	// Quaternion that rotates from body1's constraint space to world space
	const quat globalAnchorOrientationA = quatQMultQ(orientationA, joint->anchorOrientationA);

	// Store world space axis of constraint space
	const mat4 globalAnchorMatrixA = mat4Quaternion(globalAnchorOrientationA);
	joint->mTranslationAxis[0] = *((vec3 *)&globalAnchorMatrixA[0][0]);
	joint->mTranslationAxis[1] = *((vec3 *)&globalAnchorMatrixA[1][0]);
	joint->mTranslationAxis[2] = *((vec3 *)&globalAnchorMatrixA[2][0]);

	if(IsTranslationFullyConstrained()){
		// All translation locked: Setup point constraint
		joint->partPoint.CalculateConstraintProperties(*mBody1, Mat44::sRotation(rotation1), mLocalSpacePosition1, *mBody2, Mat44::sRotation(rotation2), mLocalSpacePosition2);
	}else if(IsTranslationConstrained() || mTranslationMotorActive){

		// Update world space positions (the bodies may have moved)
		Vec3 r1_plus_u, r2, u;
		GetPositionConstraintProperties(r1_plus_u, r2, u);

		// Setup axis constraint parts
		for(int i = 0; i < 3; ++i){

			EAxis axis = EAxis(EAxis::TranslationX + i);

			vec3 translation_axis = mTranslationAxis[i];

			// Calculate displacement along this axis
			float d = vec3Dot(translation_axis, u);
			mDisplacement[i] = d; // Store for SolveVelocityConstraint

			// Setup limit constraint
			bool constraint_active = false;
			float constraint_value = 0.f;

			if(IsFixedAxis(axis)){
				// When constraint is fixed it is always active
				constraint_value = d - mLimitMin[i];
				constraint_active = true;
			}else if(!IsFreeAxis(axis)){
				// When constraint is limited, it is only active when outside of the allowed range
				if(d <= mLimitMin[i]){
					constraint_value = d - mLimitMin[i];
					constraint_active = true;
				}else if(d >= mLimitMax[i]){
					constraint_value = d - mLimitMax[i];
					constraint_active = true;
				}
			}

			if(constraint_active){
				joint->partTranslation[i].CalculateConstraintPropertiesWithSettings(inDeltaTime, *mBody1, r1_plus_u, *mBody2, r2, translation_axis, 0.f, constraint_value, mLimitsSpringSettings[i]);
			}else{
				joint->partTranslation[i].Deactivate();
            }

			// Setup motor constraint
			switch (mMotorState[i]){
				case EMotorState::Off:
					if(HasFriction(axis)){
						joint->partRotationMotor[i].CalculateConstraintProperties(*mBody1, r1_plus_u, *mBody2, r2, translation_axis);
					}else{
						joint->partRotationMotor[i].Deactivate();
					}
					break;
				case EMotorState::Velocity:
					joint->partRotationMotor[i].CalculateConstraintProperties(*mBody1, r1_plus_u, *mBody2, r2, translation_axis, -mTargetVelocity[i]);
					break;
				case EMotorState::Position:
					{
						const SpringSettings &spring_settings = mMotorSettings[i].mSpringSettings;
						if(spring_settings.HasStiffness()){
							joint->partRotationMotor[i].CalculateConstraintPropertiesWithSettings(inDeltaTime, *mBody1, r1_plus_u, *mBody2, r2, translation_axis, 0.f, translation_axis.Dot(u) - mTargetPosition[i], spring_settings);
						}else{
							joint->partRotationMotor[i].Deactivate();
						}
					}
			}

		}

	}

	// Setup rotation constraints
	if(IsRotationFullyConstrained()){
		// All rotation locked: Setup rotation contraint
		joint->partRotationEuler.CalculateConstraintProperties(*mBody1, Mat44::sRotation(mBody1->GetRotation()), *mBody2, Mat44::sRotation(mBody2->GetRotation()));
	}else if(IsRotationConstrained() || mRotationMotorActive){

		// GetRotationInConstraintSpace without redoing the calculation of constraint_body1_to_world
		const quat globalAnchorOrientationB = mBody2->GetRotation() * mConstraintToBody2;
		quat q = constraint_body1_to_world.Conjugated() * constraint_body2_to_world;

		// Use swing twist constraint part
		if(IsRotationConstrained()){
			joint->partSwingTwist.CalculateConstraintProperties(*mBody1, *mBody2, q, constraint_body1_to_world);
		}else{
			joint->partSwingTwist.Deactivate();
		}

		if(mRotationMotorActive){

			// Calculate rotation motor axis
			Mat44 ws_axis = Mat44::sRotation(constraint_body2_to_world);
			for (int i = 0; i < 3; ++i){
				mRotationAxis[i] = ws_axis.GetColumn3(i);
			}

			// Get target orientation along the shortest path from q
			Quat target_orientation = q.Dot(mTargetOrientation) > 0.f ? mTargetOrientation : -mTargetOrientation;

			// The definition of the constraint rotation q:
			// R2 * ConstraintToBody2 = R1 * ConstraintToBody1 * q (1)
			//
			// R2' is the rotation of body 2 when reaching the target_orientation:
			// R2' * ConstraintToBody2 = R1 * ConstraintToBody1 * target_orientation (2)
			//
			// The difference in body 2 space:
			// R2' = R2 * diff_body2 (3)
			//
			// We want to specify the difference in the constraint space of body 2:
			// diff_body2 = ConstraintToBody2 * diff * ConstraintToBody2^* (4)
			//
			// Extracting R2' from 2: R2' = R1 * ConstraintToBody1 * target_orientation * ConstraintToBody2^* (5)
			// Combining 3 & 4: R2' = R2 * ConstraintToBody2 * diff * ConstraintToBody2^* (6)
			// Combining 1 & 6: R2' = R1 * ConstraintToBody1 * q * diff * ConstraintToBody2^* (7)
			// Combining 5 & 7: R1 * ConstraintToBody1 * target_orientation * ConstraintToBody2^* = R1 * ConstraintToBody1 * q * diff * ConstraintToBody2^*
			// <=> target_orientation = q * diff
			// <=> diff = q^* * target_orientation
			Quat diff = q.Conjugated() * target_orientation;

			// Project diff so that only rotation around axis that have a position motor are remaining
			Quat projected_diff;
			switch(mRotationPositionMotorActive){
				case 0x01:
					// Keep only rotation around X
					projected_diff = diff.GetTwist(Vec3::sAxisX());
					break;
				case 0x02:
					// Keep only rotation around Y
					projected_diff = diff.GetTwist(Vec3::sAxisY());
					break;
				case 0x04:
					// Keep only rotation around Z
					projected_diff = diff.GetTwist(Vec3::sAxisZ());
					break;
				case 0x03:
					// Remove rotation around Z
					// q = swing_xy * twist_z <=> swing_xy = q * twist_z^*
					projected_diff = diff * diff.GetTwist(Vec3::sAxisZ()).Conjugated();
					break;
				case 0x05:
					// Remove rotation around Y
					// q = swing_xz * twist_y <=> swing_xz = q * twist_y^*
					projected_diff = diff * diff.GetTwist(Vec3::sAxisY()).Conjugated();
					break;
				case 0x06:
					// Remove rotation around X
					// q = swing_yz * twist_x <=> swing_yz = q * twist_x^*
					projected_diff = diff * diff.GetTwist(Vec3::sAxisX()).Conjugated();
					break;
				///case 0b111:
				default: // All motors off is handled here but the results are unused
					// Keep entire rotation
					projected_diff = diff;
					break;
			}

			// Approximate error angles
			// The imaginary part of a quaternion is rotation_axis * sin(angle / 2)
			// If angle is small, sin(x) = x so angle[i] ~ 2.0f * rotation_axis[i]
			// We'll be making small time steps, so if the angle is not small at least the sign will be correct and we'll move in the right direction
			Vec3 rotation_error = -2.f * projected_diff.GetXYZ();

			// Setup motors
			for(int i = 0; i < 3; ++i){

				EAxis axis = EAxis(EAxis::RotationX + i);

				Vec3 rotation_axis = mRotationAxis[i];

				switch(mMotorState[axis]){
					case EMotorState::Off:
						if(HasFriction(axis)){
							joint->partRotationMotor[i].CalculateConstraintProperties(*mBody1, *mBody2, rotation_axis);
						}else{
							joint->partRotationMotor[i].Deactivate();
						}
					break;
					case EMotorState::Velocity:
						joint->partRotationMotor[i].CalculateConstraintProperties(*mBody1, *mBody2, rotation_axis, -mTargetAngularVelocity[i]);
						break;
					case EMotorState::Position:
						{
							const SpringSettings &spring_settings = mMotorSettings[axis].mSpringSettings;
							if(spring_settings.HasStiffness()){
								joint->partRotationMotor[i].CalculateConstraintPropertiesWithSettings(inDeltaTime, *mBody1, *mBody2, rotation_axis, 0.f, rotation_error[i], spring_settings);
							}else{
								joint->partRotationMotor[i].Deactivate();
							}
						}
				}

			}

		}

	}

}

void SixDOFConstraint::WarmStartVelocityConstraint(float inWarmStartImpulseRatio)
{
	// Warm start translation motors
	if (mTranslationMotorActive)
		for (int i = 0; i < 3; ++i)
			if (mMotorTranslationConstraintPart[i].IsActive())
				mMotorTranslationConstraintPart[i].WarmStart(*mBody1, *mBody2, mTranslationAxis[i], inWarmStartImpulseRatio);

	// Warm start rotation motors
	if (mRotationMotorActive)
		for (AngleConstraintPart &c : mMotorRotationConstraintPart)
			if (c.IsActive())
				c.WarmStart(*mBody1, *mBody2, inWarmStartImpulseRatio);

	// Warm start rotation constraints
	if (IsRotationFullyConstrained())
		mRotationConstraintPart.WarmStart(*mBody1, *mBody2, inWarmStartImpulseRatio);
	else if (IsRotationConstrained())
		mSwingTwistConstraintPart.WarmStart(*mBody1, *mBody2, inWarmStartImpulseRatio);

	// Warm start translation constraints
	if (IsTranslationFullyConstrained())
		mPointConstraintPart.WarmStart(*mBody1, *mBody2, inWarmStartImpulseRatio);
	else if (IsTranslationConstrained())
		for (int i = 0; i < 3; ++i)
			if (mTranslationConstraintPart[i].IsActive())
				mTranslationConstraintPart[i].WarmStart(*mBody1, *mBody2, mTranslationAxis[i], inWarmStartImpulseRatio);
}

bool SixDOFConstraint::SolveVelocityConstraint(float inDeltaTime)
{
	bool impulse = false;

	// Solve translation motor
	if (mTranslationMotorActive)
		for (int i = 0; i < 3; ++i)
			if (mMotorTranslationConstraintPart[i].IsActive())
				switch (mMotorState[i])
				{
				case EMotorState::Off:
				{
					// Apply friction only
					float max_lambda = mMaxFriction[i] * inDeltaTime;
					impulse |= mMotorTranslationConstraintPart[i].SolveVelocityConstraint(*mBody1, *mBody2, mTranslationAxis[i], -max_lambda, max_lambda);
					break;
				}

				case EMotorState::Velocity:
				case EMotorState::Position:
					// Drive motor
					impulse |= mMotorTranslationConstraintPart[i].SolveVelocityConstraint(*mBody1, *mBody2, mTranslationAxis[i], inDeltaTime * mMotorSettings[i].mMinForceLimit, inDeltaTime * mMotorSettings[i].mMaxForceLimit);
					break;
				}

	// Solve rotation motor
	if (mRotationMotorActive)
		for (int i = 0; i < 3; ++i)
		{
			EAxis axis = EAxis(EAxis::RotationX + i);
			if (mMotorRotationConstraintPart[i].IsActive())
				switch (mMotorState[axis])
				{
				case EMotorState::Off:
				{
					// Apply friction only
					float max_lambda = mMaxFriction[axis] * inDeltaTime;
					impulse |= mMotorRotationConstraintPart[i].SolveVelocityConstraint(*mBody1, *mBody2, mRotationAxis[i], -max_lambda, max_lambda);
					break;
				}

				case EMotorState::Velocity:
				case EMotorState::Position:
					// Drive motor
					impulse |= mMotorRotationConstraintPart[i].SolveVelocityConstraint(*mBody1, *mBody2, mRotationAxis[i], inDeltaTime * mMotorSettings[axis].mMinTorqueLimit, inDeltaTime * mMotorSettings[axis].mMaxTorqueLimit);
					break;
				}
		}

	// Solve rotation constraint
	if (IsRotationFullyConstrained())
		impulse |= mRotationConstraintPart.SolveVelocityConstraint(*mBody1, *mBody2);
	else if (IsRotationConstrained())
		impulse |= mSwingTwistConstraintPart.SolveVelocityConstraint(*mBody1, *mBody2);

	// Solve position constraint
	if (IsTranslationFullyConstrained())
		impulse |= mPointConstraintPart.SolveVelocityConstraint(*mBody1, *mBody2);
	else if (IsTranslationConstrained())
		for (int i = 0; i < 3; ++i)
			if (mTranslationConstraintPart[i].IsActive())
			{
				// If the axis is not fixed it must be limited (or else the constraint would not be active)
				// Calculate the min and max constraint force based on on which side we're limited
				float limit_min = -FLT_MAX, limit_max = FLT_MAX;
				if (!IsFixedAxis(EAxis(EAxis::TranslationX + i)))
				{
					JPH_ASSERT(!IsFreeAxis(EAxis(EAxis::TranslationX + i)));
					if (mDisplacement[i] <= mLimitMin[i])
						limit_min = 0;
					else if (mDisplacement[i] >= mLimitMax[i])
						limit_max = 0;
				}

				impulse |= mTranslationConstraintPart[i].SolveVelocityConstraint(*mBody1, *mBody2, mTranslationAxis[i], limit_min, limit_max);
			}

	return impulse;
}

bool SixDOFConstraint::SolvePositionConstraint(float inDeltaTime, float inBaumgarte)
{
	bool impulse = false;

	if (IsRotationFullyConstrained())
	{
		// Rotation locked: Solve rotation constraint

		// Inverse of initial rotation from body 1 to body 2 in body 1 space
		// Definition of initial orientation r0: q2 = q1 r0
		// Initial rotation (see: GetRotationInConstraintSpace): q2 = q1 c1 c2^-1
		// So: r0^-1 = (c1 c2^-1)^-1 = c2 * c1^-1
		Quat constraint_to_body1 = mConstraintToBody1 * Quat::sEulerAngles(GetRotationLimitsMin());
		Quat inv_initial_orientation = mConstraintToBody2 * constraint_to_body1.Conjugated();

		// Solve rotation violations
		mRotationConstraintPart.CalculateConstraintProperties(*mBody1, Mat44::sRotation(mBody1->GetRotation()), *mBody2, Mat44::sRotation(mBody2->GetRotation()));
		impulse |= mRotationConstraintPart.SolvePositionConstraint(*mBody1, *mBody2, inv_initial_orientation, inBaumgarte);
	}
	else if (IsRotationConstrained())
	{
		// Rotation partially constraint

		// Solve rotation violations
		Quat q = GetRotationInConstraintSpace();
		impulse |= mSwingTwistConstraintPart.SolvePositionConstraint(*mBody1, *mBody2, q, mConstraintToBody1, mConstraintToBody2, inBaumgarte);
	}

	// Solve position violations
	if (IsTranslationFullyConstrained())
	{
		// Translation locked: Solve point constraint
		Vec3 local_space_position1 = mLocalSpacePosition1 + mConstraintToBody1 * GetTranslationLimitsMin();
		mPointConstraintPart.CalculateConstraintProperties(*mBody1, Mat44::sRotation(mBody1->GetRotation()), local_space_position1, *mBody2, Mat44::sRotation(mBody2->GetRotation()), mLocalSpacePosition2);
		impulse |= mPointConstraintPart.SolvePositionConstraint(*mBody1, *mBody2, inBaumgarte);
	}
	else if (IsTranslationConstrained())
	{
		// Translation partially locked: Solve per axis
		for (int i = 0; i < 3; ++i)
			if (mLimitsSpringSettings[i].mFrequency <= 0.0f) // If not soft limit
			{
				// Update world space positions (the bodies may have moved)
				Vec3 r1_plus_u, r2, u;
				GetPositionConstraintProperties(r1_plus_u, r2, u);

				// Quaternion that rotates from body1's constraint space to world space
				Quat constraint_body1_to_world = mBody1->GetRotation() * mConstraintToBody1;

				// Calculate axis
				Vec3 translation_axis;
				switch (i)
				{
				case 0:							translation_axis = constraint_body1_to_world.RotateAxisX(); break;
				case 1:							translation_axis = constraint_body1_to_world.RotateAxisY(); break;
				default:	JPH_ASSERT(i == 2); translation_axis = constraint_body1_to_world.RotateAxisZ(); break;
				}

				// Determine position error
				float error = 0.0f;
				EAxis axis(EAxis(EAxis::TranslationX + i));
				if (IsFixedAxis(axis))
					error = u.Dot(translation_axis) - mLimitMin[axis];
				else if (!IsFreeAxis(axis))
				{
					float displacement = u.Dot(translation_axis);
					if (displacement <= mLimitMin[axis])
						error = displacement - mLimitMin[axis];
					else if (displacement >= mLimitMax[axis])
						error = displacement - mLimitMax[axis];
				}

				if (error != 0.0f)
				{
					// Setup axis constraint part and solve it
					mTranslationConstraintPart[i].CalculateConstraintProperties(*mBody1, r1_plus_u, *mBody2, r2, translation_axis);
					impulse |= mTranslationConstraintPart[i].SolvePositionConstraint(*mBody1, *mBody2, translation_axis, error, inBaumgarte);
				}
			}
	}

	return impulse;
}
**/
