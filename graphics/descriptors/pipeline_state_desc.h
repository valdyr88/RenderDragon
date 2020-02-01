#ifndef PIPELINE_STATE_DESC_H
#define PIPELINE_STATE_DESC_H

#include "../utils/types/types.h"
#include "../utils/pointers.h"
#include "graphics_enums.h"

struct SAttachmentBlendDesc { //Attachment == RenderTarget in DX
	bool blendEnable = false;
	bool logicOperationEnable = false;

	EBlendFactor srcBlend = EBlendFactor::SrcAlpha;
	EBlendFactor dstBlend = EBlendFactor::OneMinusSrcAlpha;
	EBlendOperation blendOp = EBlendOperation::Add;
	EBlendFactor srcBlendAlpha = EBlendFactor::One;
	EBlendFactor dstBlendAlpha = EBlendFactor::OneMinusSrcAlpha;
	EBlendOperation blendOpAlpha = EBlendOperation::Add;

	ELogicOperation logicOp = ELogicOperation::NoOp;

	uint8 attachmentWriteMask = 0xff;

	bool operator ==(const SAttachmentBlendDesc& other) const {
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
	bool operator != (const SAttachmentBlendDesc& other) const {
		return !(*this == other);
	}

	SAttachmentBlendDesc& operator = (const SAttachmentBlendDesc& other) {
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

	SAttachmentBlendDesc() = default;

	SAttachmentBlendDesc(bool enableBlend, bool enableLogicOp,
		EBlendFactor src, EBlendFactor dst,
		EBlendOperation op,
		EBlendFactor srcA, EBlendFactor dstA,
		EBlendOperation opA,
		ELogicOperation logOp,
		uint8 writeMask) :
		blendEnable(enableBlend), logicOperationEnable(enableLogicOp),
		srcBlend(src), dstBlend(dst), blendOp(op), srcBlendAlpha(srcA), dstBlendAlpha(dstA), blendOpAlpha(opA),
		logicOp(logOp), attachmentWriteMask(writeMask)
	{}
};


struct SBlendStateDesc {
	bool alphaToCoverageEnable = false;
	bool independentBlendEnable = false;

	SAttachmentBlendDesc attachmentBlends[RD_MAX_RENDER_ATTACHMENTS];

	bool operator == (const SBlendStateDesc& other) const {
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
	bool operator != (const SBlendStateDesc& other) const {
		return !(*this == other);
	}

	SBlendStateDesc& operator = (const SBlendStateDesc& other) {
		alphaToCoverageEnable = other.alphaToCoverageEnable;
		independentBlendEnable = other.independentBlendEnable;
		for (int i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i)
			attachmentBlends[i] = other.attachmentBlends[i];

		return *this;
	}
};

struct SRasterizerStateDesc {
	EFillMode fillMode = EFillMode::Solid;
	ECullMode cullMode = ECullMode::BackFaces;
	EFrontFace frontFace = EFrontFace::CounterClockwise;
	bool depthClipEnable = false;
	bool antialiasedLineEnable = false;
	int32 depthBias = 0;
	float depthBiasClamp = 0.0f;
	float slopeScaledDepthBias = 0.0f;

	bool operator == (const SRasterizerStateDesc& other) const {
		return fillMode == other.fillMode &&
			cullMode == other.cullMode &&
			frontFace == other.frontFace &&
			depthClipEnable == other.depthClipEnable &&
			antialiasedLineEnable == other.antialiasedLineEnable &&
			depthBias == other.depthBias &&
			depthBiasClamp == other.depthBiasClamp &&
			slopeScaledDepthBias == other.slopeScaledDepthBias;
	}
	bool operator != (const SRasterizerStateDesc& other) const {
		return !(*this == other);
	}

	SRasterizerStateDesc& operator = (const SRasterizerStateDesc& other) {
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

struct SStencilOpDesc {
	EStencilOp stencilFailOp = EStencilOp::Keep;
	EStencilOp stencilDepthFailOp = EStencilOp::Keep;
	EStencilOp stencilPassOp = EStencilOp::Keep;
	EComparisonOp stencilFunc = EComparisonOp::Always;
	int32 referenceValue = 0x0;
	uint32 ANDMask = 0xffffffff;
	uint32 writeMask = 0xffffffff;

	bool operator == (const SStencilOpDesc& other) const {
		return stencilFailOp == other.stencilFailOp &&
			stencilDepthFailOp == other.stencilDepthFailOp &&
			stencilPassOp == other.stencilPassOp &&
			stencilFunc == other.stencilFunc &&
			referenceValue == other.referenceValue &&
			ANDMask == other.ANDMask;
	}
	bool operator != (const SStencilOpDesc& other) const {
		return !(*this == other);
	}

	SStencilOpDesc& operator = (const SStencilOpDesc& other) {
		stencilFailOp = other.stencilFailOp;
		stencilDepthFailOp = other.stencilDepthFailOp;
		stencilPassOp = other.stencilPassOp;
		stencilFunc = other.stencilFunc;
		referenceValue = other.referenceValue;
		ANDMask = other.ANDMask;

		return *this;
	}
};

struct SStencilStateDesc {
	bool enable = false;
	uint8 stencilReadMask = 0xff;
	uint8 stencilWriteMask = 0xff;
	SStencilOpDesc frontFace;
	SStencilOpDesc backFace;

	bool operator == (const SStencilStateDesc& other) const {
		return enable == other.enable &&
			stencilReadMask == other.stencilReadMask &&
			stencilWriteMask == other.stencilWriteMask &&
			frontFace == other.frontFace &&
			backFace == other.backFace;
	}
	bool operator != (const SStencilStateDesc& other) const {
		return !(*this == other);
	}

	SStencilStateDesc& operator = (const SStencilStateDesc& other) {
		enable = other.enable;
		stencilReadMask = other.stencilReadMask;
		stencilWriteMask = other.stencilWriteMask;
		frontFace = other.frontFace;
		backFace = other.backFace;

		return *this;
	}
};

struct SDepthStateDesc {
	bool enable = false;
	bool depthWriteEnable = false;
	EComparisonOp depthFunc = EComparisonOp::Less;

	bool operator == (const SDepthStateDesc& other) const {
		return enable == other.enable &&
			depthWriteEnable == other.depthWriteEnable &&
			depthFunc == other.depthFunc;
	}
	bool operator != (const SDepthStateDesc& other) const {
		return !(*this == other);
	}

	SDepthStateDesc& operator = (const SDepthStateDesc& other) {
		enable = other.enable;
		depthWriteEnable = other.depthWriteEnable;
		depthFunc = other.depthFunc;

		return *this;
	}
};

struct SSampleDesc {
	uint32 sampleMask = 0xffffffff;
	uint8 count = 1;
	//uint8 quality = 0;

	bool operator == (const SSampleDesc& other) const { return count == other.count && sampleMask == other.sampleMask; }
	bool operator != (const SSampleDesc& other) const {
		return !(*this == other);
	}

	SSampleDesc& operator = (const SSampleDesc& other) {
		count = other.count;
		sampleMask = other.sampleMask;
		return *this;
	}
};

struct SViewport {
	float x = 0.0f, y = 0.0f, width = 0.0f, height = 0.0f;

	bool operator == (const SViewport& other) const {
		return (x == other.x) && (y == other.y) && (width == other.width) && (height == other.height);
	}
	bool operator != (const SViewport& other) const {
		return !(*this == other);
	}

	SViewport& operator = (const SViewport& other) {
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;

		return *this;
	}
};

struct SScissorTest {
	bool enable = false;
	float x = 0.0f, y = 0.0f, width = 0.0f, height = 0.0f;

	bool operator == (const SScissorTest& other) const {
		if (enable != other.enable) return false;
		if (enable == false) return true;
		return (x == other.x) && (y == other.y) && (width == other.width) && (height == other.height);
	}
	bool operator != (const SScissorTest& other) const {
		return !(*this == other);
	}

	SScissorTest& operator = (const SScissorTest& other) {
		enable = other.enable;
		x = other.x;
		y = other.y;
		width = other.width;
		height = other.height;

		return *this;
	}
};

struct SViewports {
	uint8 numViewports = 1;
	SViewport viewports[RD_MAX_VIEWPORTS];
	SScissorTest scissorTest[RD_MAX_VIEWPORTS];

	bool operator == (const SViewports& other) const {
		if (numViewports != other.numViewports) return false;
		for (int i = 0; i < numViewports; ++i)
			if (viewports[i] != other.viewports[i]) return false;
		for (int i = 0; i < numViewports; ++i)
			if (scissorTest[i] != other.scissorTest[i]) return false;
		return true;
	}
	bool operator != (const SViewports& other) const {
		return !(*this == other);
	}

	SViewports& operator = (const SViewports& other) {
		numViewports = other.numViewports;
		for (int i = 0; i < numViewports; ++i) {
			viewports[i] = other.viewports[i];
			scissorTest[i] = other.scissorTest[i];
		}

		return *this;
	}
};

class CShaderProgram;
class CRenderPass;

struct SPipelineStateDesc {
	SharedPtr<CShaderProgram> shader = nullptr;
	SharedPtr<CRenderPass> renderPass = nullptr;
	SBlendStateDesc blendDesc;
	SRasterizerStateDesc rasterizerDesc;
	SDepthStateDesc depthDesc;
	SStencilStateDesc stencilDesc;
	EPrimitiveTopology primitiveTopology;
	SSampleDesc samplingDesc;
	SViewports viewports;

	bool operator == (const SPipelineStateDesc& other) const {
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
	bool operator != (const SPipelineStateDesc& other) const {
		return !(*this == other);
	}

	SPipelineStateDesc& operator = (const SPipelineStateDesc& other) {
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

	const SAttachmentBlendDesc None = SAttachmentBlendDesc(
		false,
		false,
		EBlendFactor::SrcAlpha,
		EBlendFactor::OneMinusSrcAlpha,
		EBlendOperation::Add,
		EBlendFactor::One,
		EBlendFactor::OneMinusSrcAlpha,
		EBlendOperation::Add,
		ELogicOperation::NoOp,
		0xff
	);

	const SAttachmentBlendDesc AlphaBlend = SAttachmentBlendDesc(
		true,
		false,
		EBlendFactor::SrcAlpha,
		EBlendFactor::OneMinusSrcAlpha,
		EBlendOperation::Add,
		EBlendFactor::One,
		EBlendFactor::OneMinusSrcAlpha,
		EBlendOperation::Add,
		ELogicOperation::NoOp,
		0xff
	);

	const SAttachmentBlendDesc PremulBlend = SAttachmentBlendDesc(
		true,
		false,
		EBlendFactor::One,
		EBlendFactor::OneMinusSrcAlpha,
		EBlendOperation::Add,
		EBlendFactor::One,
		EBlendFactor::OneMinusSrcAlpha,
		EBlendOperation::Add,
		ELogicOperation::NoOp,
		0xff
	);

	const SAttachmentBlendDesc StoreAlphaBlend = SAttachmentBlendDesc(
		true,
		false,
		EBlendFactor::SrcAlpha,
		EBlendFactor::OneMinusSrcAlpha,
		EBlendOperation::Add,
		EBlendFactor::One,
		EBlendFactor::OneMinusSrcAlpha,
		EBlendOperation::Add,
		ELogicOperation::NoOp,
		0xff
	);

	const SAttachmentBlendDesc AdditiveBlend = SAttachmentBlendDesc(
		true,
		false,
		EBlendFactor::One,
		EBlendFactor::One,
		EBlendOperation::Add,
		EBlendFactor::One,
		EBlendFactor::One,
		EBlendOperation::Add,
		ELogicOperation::NoOp,
		0xff
	);

}

#endif //PIPELINE_STATE_DESC_H
