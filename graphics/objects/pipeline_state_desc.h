#ifndef PIPELINE_STATE_DESC_H
#define PIPELINE_STATE_DESC_H

#include "../../utils/pointers.h"
#include "../enum/graphics_enums.h"
#include "../../utils/types/types.h"

struct AttachmentBlendDesc { //Attachment == RenderTarget in DX
	bool blendEnable = false;
	bool logicOperationEnable = false;

	BlendFactor srcBlend = BlendFactor::SrcAlpha;
	BlendFactor dstBlend = BlendFactor::OneMinusSrcAlpha;
	BlendOperation blendOp = BlendOperation::Add;
	BlendFactor srcBlendAlpha = BlendFactor::One;
	BlendFactor dstBlendAlpha = BlendFactor::OneMinusSrcAlpha;
	BlendOperation blendOpAlpha = BlendOperation::Add;

	LogicOperation logicOp = LogicOperation::NoOp;

	uint8 attachmentWriteMask = 0xff;

	bool operator ==(const AttachmentBlendDesc& other) const {
		return blendEnable == other.blendEnable &&
			logicOperationEnable == other.logicOperationEnable &&
			srcBlend == other.srcBlend &&
			dstBlend == other.dstBlend &&
			srcBlendAlpha == other.srcBlendAlpha &&
			dstBlendAlpha == other.dstBlendAlpha &&
			blendOp == other.blendOp &&
			blendOpAlpha == other.blendOpAlpha &&
			attachmentWriteMask == other.attachmentWriteMask;
	}
	bool operator != (const AttachmentBlendDesc& other) const {
		return !(*this == other);
	}

	AttachmentBlendDesc& operator = (const AttachmentBlendDesc& other) {
		blendEnable = other.blendEnable;
		logicOperationEnable = other.logicOperationEnable;
		srcBlend = other.srcBlend;
		dstBlend = other.dstBlend;
		blendOp = other.blendOp;
		srcBlendAlpha = other.srcBlendAlpha;
		dstBlendAlpha = other.dstBlendAlpha;
		blendOpAlpha = other.blendOpAlpha;
		logicOp = other.logicOp;
		attachmentWriteMask = other.attachmentWriteMask;

		return *this;
	}

	AttachmentBlendDesc() = default;

	AttachmentBlendDesc(bool enableBlend, bool enableLogicOp,
		BlendFactor src, BlendFactor dst,
		BlendOperation op,
		BlendFactor srcA, BlendFactor dstA,
		BlendOperation opA,
		LogicOperation logOp,
		uint8 writeMask) :
		blendEnable(enableBlend), logicOperationEnable(enableLogicOp),
		srcBlend(src), dstBlend(dst), blendOp(op), srcBlendAlpha(srcA), dstBlendAlpha(dstA), blendOpAlpha(opA),
		logicOp(logOp), attachmentWriteMask(writeMask)
	{}
};


struct BlendStateDesc {
	bool alphaToCoverageEnable = false;
	bool independentBlendEnable = false;

	AttachmentBlendDesc attachmentBlends[RD_MAX_RENDER_ATTACHMENTS];

	bool operator == (const BlendStateDesc& other) const {
		if (independentBlendEnable == true) {
			bool attSame = true;
			for (int i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i)
				attSame &= (attachmentBlends[i] == other.attachmentBlends[i]);

			return independentBlendEnable == other.independentBlendEnable &&
				alphaToCoverageEnable == other.alphaToCoverageEnable &&
				attSame;
		}
		else {
			return independentBlendEnable == other.independentBlendEnable &&
				alphaToCoverageEnable == other.alphaToCoverageEnable &&
				attachmentBlends[0] == other.attachmentBlends[0];
		}
	}
	bool operator != (const BlendStateDesc& other) const {
		return !(*this == other);
	}

	BlendStateDesc& operator = (const BlendStateDesc& other) {
		alphaToCoverageEnable = other.alphaToCoverageEnable;
		independentBlendEnable = other.independentBlendEnable;
		for (int i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i)
			attachmentBlends[i] = other.attachmentBlends[i];

		return *this;
	}
};

struct RasterizerStateDesc {
	FillMode fillMode = FillMode::Solid;
	CullMode cullMode = CullMode::BackFaces;
	FrontFace frontFace = FrontFace::CounterClockwise;
	bool depthClipEnable = false;
	bool antialiasedLineEnable = false;
	int32 depthBias = 0;
	float depthBiasClamp = 0.0f;
	float slopeScaledDepthBias = 0.0f;

	bool operator == (const RasterizerStateDesc& other) const {
		return fillMode == other.fillMode &&
			cullMode == other.cullMode &&
			frontFace == other.frontFace &&
			depthClipEnable == other.depthClipEnable &&
			antialiasedLineEnable == other.antialiasedLineEnable &&
			depthBias == other.depthBias &&
			depthBiasClamp == other.depthBiasClamp &&
			slopeScaledDepthBias == other.slopeScaledDepthBias;
	}
	bool operator != (const RasterizerStateDesc& other) const {
		return !(*this == other);
	}

	RasterizerStateDesc& operator = (const RasterizerStateDesc& other) {
		fillMode = other.fillMode;
		cullMode = other.cullMode;
		frontFace = other.frontFace;
		depthClipEnable = other.depthClipEnable;
		antialiasedLineEnable = other.antialiasedLineEnable;
		depthBias = other.depthBias;
		depthBiasClamp = other.depthBiasClamp;
		slopeScaledDepthBias = other.slopeScaledDepthBias;

		return *this;
	}
};

struct StencilOpDesc {
	StencilOp stencilFailOp = StencilOp::Keep;
	StencilOp stencilDepthFailOp = StencilOp::Keep;
	StencilOp stencilPassOp = StencilOp::Keep;
	ComparisonOp stencilFunc = ComparisonOp::Always;
	int32 referenceValue = 0x0;
	uint32 ANDMask = 0xffffffff;
	uint32 writeMask = 0xffffffff;

	bool operator == (const StencilOpDesc& other) const {
		return stencilFailOp == other.stencilFailOp &&
			stencilDepthFailOp == other.stencilDepthFailOp &&
			stencilPassOp == other.stencilPassOp &&
			stencilFunc == other.stencilFunc &&
			referenceValue == other.referenceValue &&
			ANDMask == other.ANDMask;
	}
	bool operator != (const StencilOpDesc& other) const {
		return !(*this == other);
	}

	StencilOpDesc& operator = (const StencilOpDesc& other) {
		stencilFailOp = other.stencilFailOp;
		stencilDepthFailOp = other.stencilDepthFailOp;
		stencilPassOp = other.stencilPassOp;
		stencilFunc = other.stencilFunc;
		referenceValue = other.referenceValue;
		ANDMask = other.ANDMask;

		return *this;
	}
};

struct StencilStateDesc {
	bool enable = false;
	uint8 stencilReadMask = 0xff;
	uint8 stencilWriteMask = 0xff;
	StencilOpDesc frontFace;
	StencilOpDesc backFace;

	bool operator == (const StencilStateDesc& other) const {
		return enable == other.enable &&
			stencilReadMask == other.stencilReadMask &&
			stencilWriteMask == other.stencilWriteMask &&
			frontFace == other.frontFace &&
			backFace == other.backFace;
	}
	bool operator != (const StencilStateDesc& other) const {
		return !(*this == other);
	}

	StencilStateDesc& operator = (const StencilStateDesc& other) {
		enable = other.enable;
		stencilReadMask = other.stencilReadMask;
		stencilWriteMask = other.stencilWriteMask;
		frontFace = other.frontFace;
		backFace = other.backFace;

		return *this;
	}
};

struct DepthStateDesc {
	bool enable = false;
	bool depthWriteEnable = false;
	ComparisonOp depthFunc = ComparisonOp::Less;

	bool operator == (const DepthStateDesc& other) const {
		return enable == other.enable &&
			depthWriteEnable == other.depthWriteEnable &&
			depthFunc == other.depthFunc;
	}
	bool operator != (const DepthStateDesc& other) const {
		return !(*this == other);
	}

	DepthStateDesc& operator = (const DepthStateDesc& other) {
		enable = other.enable;
		depthWriteEnable = other.depthWriteEnable;
		depthFunc = other.depthFunc;

		return *this;
	}
};

struct SampleDesc {
	uint32 sampleMask = 0xffffffff;
	uint8 count = 1;
	//uint8 quality = 0;

	bool operator == (const SampleDesc& other) const { return count == other.count && sampleMask == other.sampleMask; }
	bool operator != (const SampleDesc& other) const {
		return !(*this == other);
	}

	SampleDesc& operator = (const SampleDesc& other) {
		count = other.count;
		sampleMask = other.sampleMask;
		return *this;
	}
};

struct Viewport {
	float x = 0.0f, y = 0.0f, width = 0.0f, height = 0.0f;

	bool operator == (const Viewport& other) const {
		return (x == other.x) && (y == other.y) && (width == other.width) && (height == other.height);
	}
	bool operator != (const Viewport& other) const {
		return !(*this == other);
	}

	Viewport& operator = (const Viewport& other) {
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;

		return *this;
	}
};

struct ScissorTest {
	bool enable = false;
	float x = 0.0f, y = 0.0f, width = 0.0f, height = 0.0f;

	bool operator == (const ScissorTest& other) const {
		if (enable != other.enable) return false;
		if (enable == false) return true;
		return (x == other.x) && (y == other.y) && (width == other.width) && (height == other.height);
	}
	bool operator != (const ScissorTest& other) const {
		return !(*this == other);
	}

	ScissorTest& operator = (const ScissorTest& other) {
		enable = other.enable;
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;

		return *this;
	}
};

struct Viewports {
	uint8 numViewports = 1;
	Viewport viewports[RD_MAX_VIEWPORTS];
	ScissorTest scissorTest[RD_MAX_VIEWPORTS];

	bool operator == (const Viewports& other) const {
		if (numViewports != other.numViewports) return false;
		for (int i = 0; i < numViewports; ++i)
			if (viewports[i] != other.viewports[i]) return false;
		for (int i = 0; i < numViewports; ++i)
			if (scissorTest[i] != other.scissorTest[i]) return false;
		return true;
	}
	bool operator != (const Viewports& other) const {
		return !(*this == other);
	}

	Viewports& operator = (const Viewports& other) {
		numViewports = other.numViewports;
		for (int i = 0; i < numViewports; ++i) {
			viewports[i] = other.viewports[i];
			scissorTest[i] = other.scissorTest[i];
		}

		return *this;
	}
};

class ShaderProgram;
class RenderPass;

struct PipelineStateDesc {
	SharedPtr<ShaderProgram> shader = nullptr;
	SharedPtr<RenderPass> renderPass = nullptr;
	BlendStateDesc blendDesc;
	RasterizerStateDesc rasterizerDesc;
	DepthStateDesc depthDesc;
	StencilStateDesc stencilDesc;
	PrimitiveTopology primitiveTopology;
	SampleDesc samplingDesc;
	Viewports viewports;

	bool operator == (const PipelineStateDesc& other) const {
		return shader == other.shader && //TODO: change pointer (address) comparison to actual object's operator ==
			renderPass == other.renderPass && //TODO: same as in line above
			blendDesc == other.blendDesc &&
			rasterizerDesc == other.rasterizerDesc &&
			depthDesc == other.depthDesc &&
			stencilDesc == other.stencilDesc &&
			primitiveTopology == other.primitiveTopology &&
			samplingDesc == other.samplingDesc &&
			viewports == other.viewports;
	}
	bool operator != (const PipelineStateDesc& other) const {
		return !(*this == other);
	}

	PipelineStateDesc& operator = (const PipelineStateDesc& other) {
		shader = other.shader;
		renderPass = other.renderPass;
		blendDesc = other.blendDesc;
		rasterizerDesc = other.rasterizerDesc;
		depthDesc = other.depthDesc;
		stencilDesc = other.stencilDesc;
		primitiveTopology = other.primitiveTopology;
		samplingDesc = other.samplingDesc;
		viewports = other.viewports;

		return *this;
	}
};


namespace BlendMode {

	const AttachmentBlendDesc None = AttachmentBlendDesc(
		false,
		false,
		BlendFactor::SrcAlpha,
		BlendFactor::OneMinusSrcAlpha,
		BlendOperation::Add,
		BlendFactor::One,
		BlendFactor::OneMinusSrcAlpha,
		BlendOperation::Add,
		LogicOperation::NoOp,
		0xff
	);

	const AttachmentBlendDesc AlphaBlend = AttachmentBlendDesc(
		true,
		false,
		BlendFactor::SrcAlpha,
		BlendFactor::OneMinusSrcAlpha,
		BlendOperation::Add,
		BlendFactor::One,
		BlendFactor::OneMinusSrcAlpha,
		BlendOperation::Add,
		LogicOperation::NoOp,
		0xff
	);

	const AttachmentBlendDesc PremulBlend = AttachmentBlendDesc(
		true,
		false,
		BlendFactor::One,
		BlendFactor::OneMinusSrcAlpha,
		BlendOperation::Add,
		BlendFactor::One,
		BlendFactor::OneMinusSrcAlpha,
		BlendOperation::Add,
		LogicOperation::NoOp,
		0xff
	);

	const AttachmentBlendDesc StoreAlphaBlend = AttachmentBlendDesc(
		true,
		false,
		BlendFactor::SrcAlpha,
		BlendFactor::OneMinusSrcAlpha,
		BlendOperation::Add,
		BlendFactor::One,
		BlendFactor::OneMinusSrcAlpha,
		BlendOperation::Add,
		LogicOperation::NoOp,
		0xff
	);

	const AttachmentBlendDesc AdditiveBlend = AttachmentBlendDesc(
		true,
		false,
		BlendFactor::One,
		BlendFactor::One,
		BlendOperation::Add,
		BlendFactor::One,
		BlendFactor::One,
		BlendOperation::Add,
		LogicOperation::NoOp,
		0xff
	);

}

#endif //PIPELINE_STATE_DESC_H
